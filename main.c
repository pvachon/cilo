/* CiscoLoad - Bootloader for Cisco Routers
 * (c) 2008 Philippe Vachon <philippe@cowpig.ca>
 * Licensed under the GNU General Public License v2
 */
#include <promlib.h>
#include <printf.h>
#include <addr.h>
#include <elf.h>
#include <elf_loader.h>
#include <string.h>

/**
 * Todo: Add these to platform-specific header
 */
#define FLASH_BASE 0x30000000
#define KERNEL_ENTRY_POINT 0x80008000

#define FS_FILE_MAGIC 0xbad00b1e

/**
 * Dump 0x10 bytes of RAM in canonical hexadecimal form
 * @param addr Starting address to dump from
 */
void hex_dump(uint32_t addr)
{
    uint8_t *rgn = (uint8_t *)addr;
    int i;

    /* print out the address of the 16 bytes of interest */
    printf("%8x " , addr);

    /* print out hex value for individual bytes */
    for (i = 0; i < 16; i++) {
        printf("%02x ", rgn[i]);
    }
    /* print out as chars */
    for (i = 0; i < 16; i++) {
        printf("%c", rgn[i] >= 32 && rgn[i] <= 126 ? rgn[i] : '.');
    }
    printf("\n");

}

struct fs_ent {
    uint32_t magic;
    uint32_t length;
    /* guesses -- one of these is probably a CRC16 + flags */
    uint32_t crc32; /* ? */
    uint32_t date; /* ? */
    char filename[48];
};

/** 
 * Check the sanity of flash -- just look for the filesystem magic number
 * in the first 4 bytes of flash
 * @param base base address of flash
 * @returns 0 on failure, 1 on success
 */
int check_flash(uint32_t base)
{
    uint32_t *ptr = (uint32_t *)base;

    if (*ptr != FS_FILE_MAGIC) {
        return 0;
    }

    return 1;
}


/**
 * Find file in flash.
 * @param filename Name of the kernel ELF file to be loaded.
 * @param base Base address of flash
 * @returns offset of the file within the flash memory space.
 */
uint32_t find_file(const char *filename, uint32_t base)
{
    /* Actual file offset */
    uint32_t offset = 0;

    uint32_t file_offset = 0;

    struct fs_ent *f = (struct fs_ent *)(base + offset);

    /* iterate over files in flash */
    while (f->magic == FS_FILE_MAGIC) {
        if (!strncmp(f->filename, filename, 48)) {
            file_offset = offset + sizeof(struct fs_ent);
            break;
        }

        offset += sizeof(struct fs_ent) + f->length;
        f = (struct fs_ent *)(base + offset);
    }

    return file_offset;
}

/**
 * Print a directory listing of all files in flash
 * @param base Base address of flash
 */
void flash_directory(uint32_t base)
{
    struct fs_ent *f = (struct fs_ent *)base;
    uint32_t offset = 0;

    /* Iterate over the files; f->magic is 0 if an invalid file is
     * found.
     */
    while (f->magic == FS_FILE_MAGIC) {
        printf("%s\n", f->filename);
        offset += sizeof(struct fs_ent) + f->length;
        f = (struct fs_ent *)(base + offset);
    }
}

/**
 * Locate the stage two loader.
 * @param base Flash base.
 * @return address of stage two loader
 */
uint32_t locate_stage_two(uint32_t base) 
{
    return find_file("ciscoload.two", base);
}

/**
 * Entry Point for CiscoLoad
 */
void start_bootloader()
{
    int r = 0;
    int f;
    char buf[48];

    /* determine amount of RAM present */
    c_putc('I');

    r = c_memsz();

    /* check flash filesystem sanity */
    c_putc('L');

    f = check_flash(FLASH_BASE);
    
    if (!f) {
        printf("\nError: Unable to find any valid flash! Aborting load.\n");
        return;
    }

    c_putc('O');

    /* locate the stage two loader */
    if (!locate_stage_two(FLASH_BASE)) {
        printf("\nError: Unable to find valid stage two loader. "
            "Aborting load.\n");
        return;
    }

    printf("\nCiscoLoader (CILO) - Linux bootloader for Cisco Routers\n");
    printf("Available RAM: %d kB\n", r);

    printf("Available files:\n");
    flash_directory(FLASH_BASE);
    printf("\nEnter filename to boot:\n> ");
    c_gets(buf, 48);
    printf("\n\nAttempting to load file %s\n", buf);

    uint32_t kernel_off = find_file(buf, FLASH_BASE);
    uint32_t loader_off = find_file("ciscoload.two", FLASH_BASE);

    if (loader_off == 0) {
        printf("Unable to find the second stage loader. Please copy the "
            "second\nstage loader to the flash filesystem (ciscoload.two).");
        return;
    }
    
    if (kernel_off == 0) {
        printf("Unable to find \"%s\" on the flash filesystem.\n", buf);
    } else {
        printf("Booting \"%s\" from flash at 0x%08x\n", buf, 
            FLASH_BASE + kernel_off);
        if (load_elf32_file(FLASH_BASE + kernel_off, FLASH_BASE + loader_off) 
            < 0) 
        {
            printf("Fatal error while loading kernel. Aborting.\n");
        }
    }

    /* return to ROMMON */
}
