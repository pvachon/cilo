/* CiscoLoad - Bootloader for Cisco Routers
 * (c) 2008 Philippe Vachon <philippe@cowpig.ca>
 * Licensed under the GNU General Public License v2
 */
#include <printf.h>
#include <addr.h>
#include <elf.h>
#include <elf_loader.h>
#include <ciloio.h>
#include <promlib.h>

/* platform-specific defines */
#include <platform.h>

#include <string.h>

/**
 * Dump 0x10 bytes of memory in canonical hexadecimal form
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

/**
 * Entry Point for CiscoLoad
 */
void start_bootloader()
{
    int r = 0;
    int f;
    char buf[129];
    char *cmd_line = (char *)MEMORY_BASE;
    char kernel[49];
    const char *cmd_line_append;

    buf[128] = '\0';
    kernel[48] = '\0';

    /* determine amount of RAM present */
    c_putc('I');

    r = c_memsz();

    /* check flash filesystem sanity */
    c_putc('L');

    f = check_flash();
    
    if (!f) {
        printf("\nError: Unable to find any valid flash! Aborting load.\n");
        return;
    }

    c_putc('O');
    platform_init();

    printf("\nCiscoLoader (CILO) - Linux bootloader for Cisco Routers\n");
    printf("Available RAM: %d kB\n", r);

    printf("Available files:\n");
    flash_directory();

enter_filename:
    printf("\nEnter filename to boot:\n> ");
    c_gets(buf, 128);

    int baud = c_baud(); /* get console baud rate */
    printf("Boot console baud rate: %d\n", baud);
    
    /* determine if a command line string has been appended to kernel name */
    if ((cmd_line_append = strchr(buf, ' ')) != NULL) {
        strcpy(cmd_line, (char *)(cmd_line_append + 1));
        /* extract the kernel file name now */
        uint32_t kernel_name_len = cmd_line_append - buf;
        strncpy(kernel, buf, kernel_name_len);
        kernel[kernel_name_len + 1] = '\0';
        /* determine if console is set in the command line; if not,
         * append it.
         */
        if (!strstr(cmd_line,"console")) {
            sprintf(cmd_line, "%s console=ttyS0,%d", cmd_line, baud);
        }

    } else {
        strncpy(kernel, buf, 48);
        sprintf(cmd_line, "console=ttyS0,%d", baud);
    }

    printf("\n\nAttempting to load file %s\n", kernel);

    struct file kernel_file = cilo_open(kernel);

    if (kernel_file.code == 0) {
        printf("Unable to find \"%s\" on the flash filesystem.\n", kernel);
    } else {
#ifdef DEBUG
        printf("DEBUG: cmd_line: %s\n", cmd_line);
#endif
        printf("Booting %s.\n", kernel);
        if (load_elf32_file(&kernel_file, cmd_line) 
            < 0) 
        {
            printf("Fatal error while loading kernel. Aborting.\n");
        }
    }
    goto enter_filename;

    /* return to ROMMON */
}
