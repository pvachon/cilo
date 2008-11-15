#ifndef _STORAGE_STORAGE_H
#define _STORAGE_STORAGE_H

#include <types.h>

struct storage_class {
    uint32_t dev_id;
    char dev_name[16];
    uint32_t start_addr;

    /* storage device manager stuff */
    struct storage_class *next;
    struct storage_class *prev;
};

void initialize_storage_manager();
void register_storage_class(struct storage_class *sto);

#endif /* _STORAGE_STORAGE_H */
