/**
 * CILO Elf Loader
 * (c) 2008 Philippe Vachoon <philippe@cowpig.ca>
 *
 * Licensed under the GNU General Public License v2. See COPYING
 * in the distribution source directory for more information.
 */
#include <elf.h>
#include <promlib.h>
#include <printf.h>
#include <ciloio.h>

/* platform-specific defines */
#include <platform.h>


/**
 * load a single ELF section into memory at address. Assumes ELF data is
 * contiguous in memory.
 * @param base location (in memory) of the ELF file
 * @param address address at which the ELF section will be loaded
 * @param file_offset offset (in bytes) in the ELF file where the section is
 * @param length Length of the section (in bytes)
 */
void load_elf32_section(struct file *fp, uint32_t address, 
    uint32_t file_offset, uint32_t length)
{
    uint8_t *elf_loc = (uint8_t *)address;

#ifdef DEBUG
    printf("Init data: %08x length %08x\n", address, length);
#endif

    cilo_seek(fp, file_offset, SEEK_SET);
    cilo_read(elf_loc, length, 1, fp); 

}

/**
 * Create an uninitialized data (.bss) region of memory.
 * @param address Start address of this region
 * @param lenght length of this region
 */
void load_elf32_uninitialized_memory(uint32_t address, uint32_t length)
{
    int i = 0;
    uint8_t *p = (uint8_t *)address;

#ifdef DEBUG
    printf("Uninit data: %08x, len %08x\n", address, length); 
#endif

    for (i = 0; i < length; i++) {
        p[i] = 0;
    }
}

/**
 * Load an ELF file into memory from the given base. Loads at
 * offset + image_size so that a later memcpy routine can be used to copy
 * things into RAM and then kick off the boot process.
 * @param base The address of the ELF file in memory
 * @param loader_addr address of the loader binary in memory
 * @return  
 */
int load_elf32_file(struct file *fp)
{
    struct elf32_header hdr;

    uint32_t mem_sz = 0;

    /* read in header entries */
    cilo_read(&hdr, sizeof(struct elf32_header), 1, fp);

    /* check the file magic */
    if (hdr.ident[0] != ELF_MAGIC_1 || hdr.ident[1] != ELF_MAGIC_2 ||
        hdr.ident[2] != ELF_MAGIC_3 || hdr.ident[3] != ELF_MAGIC_4)
    {
        printf("Bad ELF magic found. Found: %#2x %#2x %#2x %#2x.\n",
            hdr.ident[0], hdr.ident[1], hdr.ident[2], hdr.ident[3]);
        return -1;
    }
    /* check machine class: */
    if (!(hdr.ident[ELF_INDEX_CLASS] == ELF_CLASS_32 || 
        hdr.ident[ELF_INDEX_CLASS] == ELF_CLASS_64))
    {
        printf("Invalid ELF machine class found. Found: %2x.\n",
            hdr.ident[ELF_INDEX_CLASS]);
        return -1;
    }

    /* check endianess: */
    if (hdr.ident[ELF_INDEX_DATA] != ELF_DATA_MSB) {
        printf("Non-big endian ELF file detected. Aborting load.\n");
        return -1;
    }

    if (hdr.ehsize != 52 /* bytes */) {
        printf("Warning: ELF header greater than 52 bytes found. Found: %u\n",
            hdr.ehsize);
    }

    if (hdr.phnum == 0) {
        printf("Found zero segments in ELF file. Aborting load.\n");
        return -1;
    }

    int i;
    struct elf32_phdr phdr;
    uint32_t offset = 0xffffffff;

    cilo_seek(fp, hdr.phoff, SEEK_SET);
    /* read in program header(s), determine total memory size of image */
    /* TODO: figure out if there's a better way to determine this */
    for (i = 0; i < hdr.phnum; i++) {
        cilo_read(&phdr, sizeof(struct elf32_phdr), 1, fp);

        if (phdr.type != ELF_PT_LOAD) continue;

        mem_sz += phdr.memsz;

        if (phdr.paddr < offset) offset = phdr.paddr;
    }

#ifdef DEBUG
    printf("mem_sz = %08x\n", mem_sz);
#endif

    /* read the PT_LOAD segments into memory at paddr + mem_sz */
    cilo_seek(fp, hdr.phoff, SEEK_SET);
    for (i = 0; i < hdr.phnum; i++) {
        cilo_read(&phdr, sizeof(struct elf32_phdr), 1, fp);

        /* skip unloadable segments */
        if (phdr.type != ELF_PT_LOAD) continue;

        /* uint32_t leftover = phdr.memsz - phdr.filesz; */
        load_elf32_section(fp, mem_sz + phdr.paddr,
            phdr.offset, phdr.filesz);

/*        if (leftover > 0) {
            load_elf32_uninitialized_memory(mem_sz + phdr.paddr +
                phdr.filesz, leftover);
        } */

        cilo_seek(fp, hdr.phoff + sizeof(struct elf32_phdr) * (i + 1), 
            SEEK_SET);
    }

    /* assume the entry point is the smallest address we're loading */
    uint32_t load_offset = mem_sz + offset;

    printf("Loaded %d bytes at %08x.\n", mem_sz, load_offset);

    printf("Kicking into Linux.\n");

#ifdef DEBUG
    printf("load_offset = 0x%08x\n", load_offset);
    printf("hdr.entry = 0x%08x\n", hdr.entry);
    printf("mem_sz = 0x%08x\n", mem_sz);
    printf("offset = 0x%08x\n", offset);
#endif

    /* Jump to the copy routine */
    stage_two(load_offset, hdr.entry, mem_sz, offset);

    return -1; /* something failed, badly */
}
