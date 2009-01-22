/* CiscoLoad - Bootloader for Cisco Routers
 * (c) 2008 Philippe Vachon <philippe@cowpig.ca>
 * Licensed under the GNU General Public License v2
 */
#include <printf.h>
#include <addr.h>
#include <elf.h>
#include <elf_loader.h>
#include <lzma_loader.h>
#include <ciloio.h>
#include <promlib.h>

/* platform-specific defines */
#include <platform.h>

#include <string.h>

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
    printf("Available RAM: %d kB\n", r/1024);

    printf("Available files:\n");
    flash_directory();

enter_filename:
    printf("\nEnter filename to boot:\n> ");
    c_gets(buf, 128);

    int baud = c_baud(); /* get console baud rate */
    
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

    struct file kernel_file = cilo_open(kernel);

    if (kernel_file.code == -1) {
        printf("Unable to find \"%s\" on the specified filesystem.\n",
            kernel);
        goto enter_filename;
    }

    /* check if this is an LZMA-compressed kernel image. */
    if (strstr(kernel, "lzma")) {
        printf("Loading LZMA-compressed kernel image.\n");
        load_lzma(&kernel_file, LOADADDR, cmd_line);
    } else {
        printf("Booting %s.\n", kernel);
        struct elf32_header hdr;

        cilo_read(&hdr, sizeof(struct elf32_header), 1, &kernel_file);
        cilo_seek(&kernel_file, 0, SEEK_SET);

        /* check if this is a 32-bit or 64-bit kernel image. */
        if (hdr.ident[ELF_INDEX_CLASS] == ELF_CLASS_32) { 
            load_elf32_file(&kernel_file, cmd_line);
        } else {
            load_elf64_file(&kernel_file, cmd_line);
        }
    }

    printf("Fatal error while loading kernel. Aborting.\n");

    goto enter_filename;
}
