#ifndef _ELF_LOADER_H
#define _ELF_LOADER_H

#include <types.h>

void load_elf32_section(uint32_t base, uint32_t address, uint32_t file_offset, 
    uint32_t length);
void load_elf32_uninitialized_memory(uint32_t address, uint32_t length);
int load_elf32_file(uint32_t base, uint32_t loader_addr);

#endif /* _ELF_LOADER_H */
