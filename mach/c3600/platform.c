#include <types.h>
#include <mach/c3600/platform.h>
#include <mach/c3600/platio.h>
#include <printf.h>

/**
 * perform hardware-specifc initialization for this platform
 */
void platform_init()
{
}

/**
 * Perform a sanity check on flash
 * @returns 0 if no flash found, number of flash devices found otherwise
 */
uint32_t check_flash()
{
    uint32_t *ptr = (uint32_t *)FLASH_BASE;

    if (*ptr != FS_FILE_MAGIC) {
        return 0;
    }

    return 1;
    /* TODO: add support for PCMCIA flash */
}

/**
 * print a directory listing of the 'main' flash device in the system
 */

void flash_directory()
{
    struct fs_ent *f = (struct fs_ent *)FLASH_BASE;
    uint32_t offset = 0;

    /* Iterate over the files; f->magic is 0 if an invalid file is found. */
    while (f->magic == FS_FILE_MAGIC) {
        printf("%s\n", f->filename);
        offset += sizeof(struct fs_ent) + f->length;
        f = (struct fs_ent *)(FLASH_BASE + offset);
    }
}

/**
 * Locate the stage two loader for CiscoLoad for this particular platform
 * returns 0 if not found, 1 if found
 */
uint32_t locate_stage_two()
{
    return platio_find_file("ciscoload.two");
}

/**
 * Kick into the stage two loader.
 */
void stage_two(uint32_t kern_off, uint32_t kern_entry, uint32_t kern_size, 
    uint32_t kern_loadpt)
{
    uint32_t s2addr = ((uint32_t)find_file("ciscoload.two", FLASH_BASE)) +
        sizeof(struct fs_ent);


    ((void (*)(uint32_t data_offset, uint32_t data_length, uint32_t entry_pt,
        uint32_t load_offset)) (s2addr))
        (kern_off, kern_size, kern_entry, kern_loadpt);
    
}
