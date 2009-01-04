/* elf2img - convert an ELF loadable object to a raw binary image or a Cisco-
 *           specific loadabe image.
 *
 * (c) 2008 Philippe Vachoon <philippe@cowpig.ca>
 *
 * Licensed under the GNU General Public License v2. See COPYING
 * in the distribution source directory for more information.
 *
 */

#include <elf.h>
#include <types.h>

#include <mzip.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

void usage(const char *s)
{
    printf("usage: %s [-m] [elffile] [outfile]\n", s);
    printf("\t-m Generates an MZIP image\n");
    printf("\t[elffile] Input ELF file\n");
    printf("\t[outfile] Output image\n");
    printf("The parameters of the format to be converted to are determined "
        "based\non the structure of the input ELF file.\n\n");
}

void swap_elf32_program_header(struct elf32_phdr *phdr)
{
    phdr->type = SWAP_32(phdr->type);
    phdr->offset = SWAP_32(phdr->offset);
    phdr->vaddr = SWAP_32(phdr->vaddr);
    phdr->paddr = SWAP_32(phdr->paddr);
    phdr->filesz = SWAP_32(phdr->filesz);
    phdr->memsz = SWAP_32(phdr->memsz);
    phdr->flags = SWAP_32(phdr->flags);
    phdr->align = SWAP_32(phdr->align);
}

void swap_elf32_header(struct elf32_header *hdr)
{
    hdr->type = SWAP_16(hdr->type);
    hdr->machine = SWAP_16(hdr->machine);
    hdr->version = SWAP_32(hdr->version);
    hdr->entry = SWAP_32(hdr->entry);
    hdr->phoff = SWAP_32(hdr->phoff);
    hdr->shoff = SWAP_32(hdr->shoff);
    hdr->flags = SWAP_32(hdr->flags);
    hdr->ehsize = SWAP_16(hdr->ehsize);
    hdr->phentsize = SWAP_16(hdr->phentsize);
    hdr->phnum = SWAP_16(hdr->phnum);
    hdr->shentsize = SWAP_16(hdr->shentsize);
    hdr->shnum = SWAP_16(hdr->shnum);
    hdr->shstrndx = SWAP_16(hdr->shstrndx);
}

#define USAGE usage(argv[0])

int main(const int argc, const char *argv[])
{
    struct elf32_header hdr;
    struct elf32_phdr *phdr;
    int i;
    char swap = 0;
    const char *file_in = argv[argc - 2];
    const char *file_out = argv[argc - 1];
    char mzip = 0;

    printf("elf2img - Cisco Router Image Generation Utility.\n");
    printf("(c) 2009 Philippe Vachon <philippe@cowpig.ca>\n\n");

    if (argc < 2) {
        printf("Insufficient arguments.\n");
        USAGE;
        return -1;
    }

    /* parse arguments */
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-m")) {
            printf("DEBUG: generating an MZIP image as output.\n");
            mzip = 1;
            if (argc < 3) {
                printf("Error: must specify input and output files.\n");
                USAGE;
                return -1;
            }
        }
    }

    FILE *fp_in = fopen(file_in, "rb");
    if (fp_in == NULL) {
        printf("Unable to open input file, %s.\n", file_in);
        USAGE;
        return -1;
    }

    rewind(fp_in);
    /* read in header */

    fread(&hdr, sizeof(struct elf32_header), 1, fp_in);

    /* check magic */
    if (hdr.ident[0] != ELF_MAGIC_1 || hdr.ident[1] != ELF_MAGIC_2 ||
        hdr.ident[2] != ELF_MAGIC_3 || hdr.ident[3] != ELF_MAGIC_4)
    {
        printf("input file is not an ELF file.\n");
        fclose(fp_in);
        USAGE;
        return -1;
    }

    /* check endianess */
    if (hdr.ident[ELF_INDEX_DATA] == ELF_DATA_MSB) swap = 1;

    if (swap) swap_elf32_header(&hdr);

    if (hdr.phnum == 0) {
        printf("No program headers found. Aborting.\n");
        fclose(fp_in);
        USAGE;
        return -1;
    }

    /* read in all phdrs */
    if ((phdr = (struct elf32_phdr *)malloc(sizeof(struct elf32_phdr) * 
        hdr.phnum)) == NULL)
    {
        printf("error while trying to allocate %08x bytes. Aborting.\n",
            (uint32_t)(sizeof(struct elf32_phdr) * hdr.phnum));

        fclose(fp_in);
        USAGE;
        return -1;
    }

    fseek(fp_in, hdr.phoff, SEEK_SET);
    fread(phdr, hdr.phnum * sizeof(struct elf32_phdr), 1, fp_in);

    if (swap)
        for (i = 0; i < hdr.phnum; i++) 
            swap_elf32_program_header(&phdr[i]);

    /* determine size of the area we need allocated. */
    uint32_t memsz = 0;
    uint32_t min_addr = 0xffffffff;
    uint32_t max_addr = 0; 

    for (i = 0; i < hdr.phnum; i++) {
        if (phdr[i].type != ELF_PT_LOAD) continue; 
        if (phdr[i].paddr + phdr[i].memsz > max_addr) 
            max_addr = phdr[i].paddr + phdr[i].memsz;
        if (phdr[i].paddr < min_addr) min_addr = phdr[i].paddr;
    }

    memsz = max_addr - min_addr;

    if (memsz == 1) {
        printf("Allocated area is 0. Does this ELF file have any LOAD-able "
            "segments?\n");
        free(phdr);
        fclose(fp_in);
        USAGE;
        return -1;
    }

    void *img = malloc(memsz);
    if (img == NULL) {
        printf("Unable to allocate %d bytes for the image. Aborting.\n", memsz);
        free(phdr);
        fclose(fp_in);
        USAGE;
        return -1;
    }

    memset(img, 0x0, memsz);

    /* read in the code into the memory buffer */
    for (i = 0; i < hdr.phnum; i++) {
        /* ignore non-LOADable segments */
        if (phdr[i].type != ELF_PT_LOAD) continue;

        uint32_t buf_off = phdr[i].paddr - min_addr;
        fseek(fp_in, phdr[i].offset, SEEK_SET);
        fread(img + buf_off, phdr[i].filesz, 1, fp_in);
    }

    FILE *fp_out = fopen(file_out, "wb+");
    if (fp_out == NULL) {
        printf("Error while opening output file.\n");
        fclose(fp_in);
        USAGE;
        return -1;
    }

    /* construct a raw memory image */
    if (!mzip) {
        fwrite(img, memsz, 1, fp_out);
        free(phdr);
        free(img);
        fclose(fp_in);
        fclose(fp_out);
        return 0;
    }

    /* construct an mzip image */
    struct mzip_header mz;
    void *zip_buf;
    uint32_t seg_size = 0;
    mzip_initialize(&mz);

    if (mzip_codeseg_build(img, memsz,  &zip_buf, &seg_size) < 0) {
        printf("Error while building MZIP code segment. Aborting.\n");
        free(phdr);
        free(img);
        fclose(fp_in);
        fclose(fp_out);
    }

    mz.hdr_version = 1;
    mz.hdr_entrypt = hdr.entry;
    mz.hdr_flags1 = 1;
    mz.hdr_flags2 = 1;
    mz.hdr_header_size = 0x70;
    mz.hdr_loader_addr = min_addr;
    mz.hdr_flags3 = 1;
    mz.hdr_code_packed_size = seg_size;
    mz.hdr_code_unpacked_size = memsz;
    mz.hdr_memory_image_size = memsz;

    if (swap) mzip_swap(&mz);

    uint16_t crc = 0;
    mzip_calculate_crc(&mz, zip_buf, seg_size, &crc);
    mz.hdr_crc_code = swap ? SWAP_16(crc) : crc;
    
    mzip_calculate_hdr_crc(&mz, &crc);
    mz.hdr_crc_header = swap ? SWAP_16(crc) : crc;

    mzip_print_header(&mz);

    /* write out the MZIP file */
    mzip_write_header(fp_out, &mz);
    mzip_write_codeseg(fp_out, zip_buf, seg_size);

    if (zip_buf) free(zip_buf);

    free(phdr);
    fclose(fp_in);
    fclose(fp_out);
    return 0;
}
