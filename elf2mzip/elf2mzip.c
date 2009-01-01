/* elf2mzip - convert a single LOADable segment ELF file to an MZIP image.
 *
 * (c) 2008 Philippe Vachoon <philippe@cowpig.ca>
 *
 * Licensed under the GNU General Public License v2. See COPYING
 * in the distribution source directory for more information.
 *
 */
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <types.h>

#include <mzip.h>
#include <elf.h>

void usage(const char *name)
{
    printf("Usage: %s [file in] [file out]\n", name);
    printf("\t[file in] is the file to be converted to MZIP format\n");
    printf("\t[file out] is the name of the output MZIP file\n");
    printf("\nNote: the input ELF file needs a single LOADable section and \n");
    printf("      must be a 32-bit ELF file.\n");
}

int main(const int argc, const char *argv[])
{
    struct elf32_header elf_hdr;
    struct mzip_header mzip_hdr;
    FILE *fp_in, *fp_out;
    struct stat f_stat;
    bool endian_swap = FALSE;
    int i;

    printf("elf2mzip version 0.1 - convert ELF files to MZIP files.\n");

    if (argc < 2) {
        printf("Error: no file arguments specified.\n");
        usage(argv[0]);
        return -1;
    }

    if ((fp_in = fopen(argv[1], "rb")) == NULL) {
        printf("Error: unable to open specified input file %s.\n",
            argv[1]);
        usage(argv[0]);
        return -1;
    }
    
    /* check if the input file exists. If it does, delete it. */
    if (stat(argv[2], &f_stat) != -1) {
        if (unlink(argv[2]) == -1) {
            /* check errno */
            switch (errno) {
            case EACCES:
                    printf("Error: insufficient privileges to access the "
                        "specified output file %s. Aborting.\n", argv[2]);
                break;
            case EBUSY:
                printf("Error: the specified file %s is currently in use.\n",
                    argv[2]);
                break;
            case EPERM:
                printf("Error: the specified file %s is on a read-only file"
                    "system.\n", argv[2]);
                break;
            default:
                printf("An unspecified error occurred. Aborting.\n");
            }

            usage(argv[0]);
            fclose(fp_in);
            return -1;
        }
    }


    if ((fp_out = fopen(argv[2], "wb+")) == NULL) {
        printf("Error: unable to open or create output file %s.\n",
            argv[2]);
        fclose(fp_in);
        usage(argv[0]);
        return -1;
    }

    mzip_initialize(&mzip_hdr);

    fseek(fp_in, 0, SEEK_SET);

    /* read in ELF header */
    fread(&elf_hdr, sizeof(elf_hdr), 1, fp_in);

    if (elf_hdr.ident[0] != ELF_MAGIC_1 || elf_hdr.ident[1] != ELF_MAGIC_2 ||
        elf_hdr.ident[2] != ELF_MAGIC_3 || elf_hdr.ident[3] != ELF_MAGIC_4)
    {
        printf("The specified input file is not an ELF file. Aborting.\n");
        fclose(fp_in);
        fclose(fp_out);
        usage(argv[0]);
        return -1;
    }

    if (elf_hdr.ident[ELF_INDEX_CLASS] != ELF_CLASS_32) {
        printf("The specified input file is a 64-bit ELF file. MZIP does not "
            "support 64-bit files for input.");
        fclose(fp_in);
        fclose(fp_out);
        usage(argv[0]);
        return -1;
    }

    /* determine endianess of the target machine */
    if (elf_hdr.ident[ELF_INDEX_DATA] == ELF_DATA_MSB) endian_swap = TRUE;

    printf("DEBUG: endian_swap = %d\n", endian_swap);

    printf("elf_hdr.phnum = %08x\n", elf_hdr.phnum);
    printf("elf_hdr.phoff = %08x\n", elf_hdr.phoff);

    uint32_t ph_count = endian_swap ? SWAP_16(elf_hdr.phnum) : elf_hdr.phnum;
    uint32_t ph_offset = endian_swap ? SWAP_32(elf_hdr.phoff) : elf_hdr.phoff;
    
    printf("DEBUG: segments: 0x%08x; segments offset: 0x%08x\n", ph_count,
        ph_offset);

    fseek(fp_in, ph_offset, SEEK_SET);

    int ph_loadable_count = 0;
    uint32_t ph_loc = 0;

    struct elf32_phdr phdr;

    for (i = 0; i < ph_count; i++) {
        fread(&phdr, 1, sizeof(struct elf32_phdr), fp_in);

        uint32_t type = endian_swap ? SWAP_32(phdr.type) : phdr.type;


        if (type == ELF_PT_LOAD) {
            ph_loadable_count++;
            ph_loc = ftell(fp_in) - sizeof(struct elf32_phdr);
        }
    }

    if (ph_loadable_count < 1) {
        printf("No loadable ELF sections were found in the input file.\n"
            "Are you sure this is a loadable image?\n");
        fclose(fp_in);
        fclose(fp_out);
        usage(argv[0]);
        return -1;
    }

    if (ph_loadable_count > 1) {
        printf("Too many loadable ELF sections were found in the input file. "
            "Found %d sections.\n", ph_loadable_count);
        fclose(fp_in);
        fclose(fp_out);
        usage(argv[0]);
        return -1;
    }

    /* create an in-memory image of the single section header */
    fseek(fp_in, ph_loc, SEEK_SET);
    fread(&phdr, sizeof(struct elf32_phdr), 1, fp_in);
    
    uint32_t im_offset = endian_swap ? SWAP_32(phdr.offset) : phdr.offset;
    uint32_t im_file_size = endian_swap ? SWAP_32(phdr.filesz) : 
        phdr.filesz;
    void *image = malloc(im_file_size);

    printf("DEBUG: allocated %08x byte image\n", im_file_size);
    printf("DEBUG: offset in file: %08x\n", im_offset);


    /* seek to offset of data within ELF image, read it in */
    fseek(fp_in, im_offset, SEEK_SET);
    fread(image, 1, im_file_size, fp_in);

    mzip_hdr.hdr_flags1 = endian_swap ? SWAP_32(0x1u) : 0x1u;
    mzip_hdr.hdr_flags2 = endian_swap ? SWAP_32(0x1u) : 0x1u;

    mzip_hdr.hdr_version = endian_swap ? SWAP_32(0x1u) : 0x1u;
    mzip_hdr.hdr_entrypt = elf_hdr.entry;

    mzip_hdr.hdr_header_size = endian_swap ? SWAP_32(0x70u) : 0x70u;
    mzip_hdr.hdr_loader_addr = phdr.paddr;
    mzip_hdr.hdr_code_unpacked_size = phdr.filesz;
    mzip_hdr.hdr_memory_image_size = phdr.memsz;

    printf("DEBUG: phdr.paddr = %08x\n", phdr.paddr);
    printf("DEBUG: phdr.filesz = %08x\n", phdr.filesz);

    /* populate the CRCs of the inputs: */
    uint16_t mzip_hdr_crc = 0;

    void *mzip_code_seg;
    uint32_t mzip_code_seg_size = 0;

    /* build code segment in buffer */
    mzip_codeseg_build(image, im_file_size, &mzip_code_seg, 
        &mzip_code_seg_size);

    mzip_hdr.hdr_code_packed_size = endian_swap ? 
        SWAP_32(mzip_code_seg_size) : mzip_code_seg_size;

    uint16_t mzip_body_crc = 0;

    mzip_calculate_crc(&mzip_hdr, mzip_code_seg, mzip_code_seg_size, 
        &mzip_body_crc);

    mzip_hdr.hdr_crc_code = endian_swap ? SWAP_16(mzip_body_crc) :
        mzip_body_crc;

    mzip_hdr.hdr_crc_header = endian_swap ? SWAP_16(mzip_hdr.hdr_crc_header) :
        mzip_hdr.hdr_crc_header;

    mzip_calculate_hdr_crc(&mzip_hdr, &mzip_hdr_crc); 
    mzip_hdr.hdr_crc_header = endian_swap ? SWAP_16(mzip_hdr_crc) :
        mzip_hdr_crc;

    printf("DEBUG: CRCs: header: %04x file: %04x\n", mzip_hdr_crc, 
        mzip_body_crc);

    mzip_print_header(&mzip_hdr);

    mzip_write_header(fp_out, &mzip_hdr);
    mzip_write_codeseg(fp_out, mzip_code_seg, mzip_code_seg_size);
    return 0;
}
