#ifndef _ELF_LOADER_H
#define _ELF_LOADER_H

#include <types.h>
#include <ciloio.h>

void load_elf32_section(struct file *fp, uint32_t address, 
    uint32_t file_offset, uint32_t length);
void load_elf32_uninitialized_memory(uint32_t address, uint32_t length);
int load_elf32_file(struct file *fp);

#endif /* _ELF_LOADER_H */
