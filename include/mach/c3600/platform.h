#ifndef _INCLUDE_MACH_C3600_PLATFORM_H
#define _INCLUDE_MACH_C3600_PLATFORM_H

#include <types.h>

#define FLASH_BASE 0x30000000
#define KERNEL_ENTRY_POINT 0x80008000
#define MEMORY_BASE 0x80000000

void platform_init();
uint32_t check_flash();
void flash_directory();
uint32_t locate_stage_two();
void stage_two(uint32_t kern_off, uint32_t kern_entry, uint32_t kern_size);

#endif /* _INCLUDE_MACH_C3600_PLATFORM_H */
