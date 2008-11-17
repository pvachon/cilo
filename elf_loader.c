#include <elf.h>
#include <promlib.h>
#include <printf.h>

/* platform-specific defines */
#include <platform.h>


void read(void *ptr, uint32_t size, uint32_t count, uint32_t base, 
    uint32_t offset);

/**
 * load a single ELF section into memory at address. Assumes ELF data is
 * contiguous in memory.
 * @param base location (in memory) of the ELF file
 * @param address address at which the ELF section will be loaded
 * @param file_offset offset (in bytes) in the ELF file where the section is
 * @param length Length of the section (in bytes)
 */
void load_elf32_section(uint32_t base, uint32_t address, 
    uint32_t file_offset, uint32_t length)
{
    uint8_t *elf_loc = (uint8_t *)address;

    read(elf_loc, length, 1, base, file_offset); 

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

    for (i = 0; i < length; i++) {
        p[i] = 0;
    }
}

/**
 * Read data into a given pointer
 * @param ptr Pointer to write data out to
 * @param size Size of the data to be read
 * @param count number of elements to read
 * @param base base address to start read from
 * @param offset offset from base to read from
 */
void read(void *ptr, uint32_t size, uint32_t count, uint32_t base, 
    uint32_t offset)
{
    uint8_t *data = (uint8_t *)ptr;
    uint8_t *src = (uint8_t *)(base + offset);
    int i;

    for (i = 0; i < size * count; i++) {
        data[i] = src[i];
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
int load_elf32_file(uint32_t base, uint32_t loader_addr)
{
    struct elf32_header hdr;

    uint32_t mem_sz = 0;

    /* read in header entries */
    read(&hdr, sizeof(struct elf32_header), 1, base, 0);

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


    if (hdr.machine != ELF_MACH_MIPS || hdr.machine != ELF_MACH_MIPS_R4K_BE ||
        hdr.machine != 0x1e)
    {
        printf("Warning: Unexpected machine type %#4x found.\n", hdr.machine);
    }

    if (hdr.ehsize != 52 /* bytes */) {
        printf("Warning: ELF header greater than 52 bytes found. Found: %u\n",
            hdr.ehsize);
    }

    if (hdr.shnum == 0) {
        printf("Can't be zero section headers in a kernel image! Aborting.\n");
        return -1;
    }

    int i;
    struct elf32_phdr phdr;
    uint32_t ph_offset = hdr.phoff;

    /* read in program header(s), determine total memory size of image */
    /* TODO: figure out if there's a better way to determine this */
    for (i = 0; i < hdr.phnum; i++) {
        read(&phdr, sizeof(struct elf32_phdr), 1, base, ph_offset);

        mem_sz += phdr.memsz;

        /*increment program header offset */
        ph_offset += sizeof(struct elf32_phdr);
    }

    /* read the PT_LOAD segments into memory at paddr + mem_sz
     */
    ph_offset = hdr.phoff;
    for (i = 0; i < hdr.phnum; i++) {
        read(&phdr, sizeof(struct elf32_phdr), 1, base, ph_offset);

        /* skip unloadable segments */
        if (phdr.type != ELF_PT_LOAD) continue;

        uint32_t leftover = phdr.memsz - phdr.filesz;
        load_elf32_section(base, mem_sz + phdr.paddr,
            phdr.offset, phdr.filesz);

        if (leftover > 0) {
            load_elf32_uninitialized_memory(mem_sz + phdr.paddr +
                phdr.filesz, leftover);
        }
    }

    /* assume the entry point is the smallest address we're loading */
    uint32_t load_offset = mem_sz + hdr.entry;

    printf("Loaded %d bytes at %08x.\n", mem_sz, load_offset);

    printf("Kicking into Linux.\n");

#ifdef DEBUG
    printf("bootcpy: %08x, kdataoffset: %08x, kdatalength: %08x\n",
        loader_addr, load_offset, mem_sz);
    printf("kentrypt: %08x, kloadoffset: %08x\n", hdr.entry, hdr.entry);
#endif

    /* Jump to the copy routine */
/*    asm (".set noreorder\n"
         "move $k0, %[bootcpy]\n"
         "move $a0, %[kdataoffset]\n"
         "move $a1, %[kdatalength]\n"
         "move $a2, %[kentrypt]\n"
         "move $a3, %[kloadoffset]\n"
         "jr $k0\n"
         " nop\n"
         : 
         : [bootcpy] "r"(loader_addr), [kdataoffset] "r"(load_offset),
           [kdatalength] "r"(mem_sz), [kentrypt]"r"(hdr.entry),
           [kloadoffset] "r"(hdr.entry)
         : "k0", "a0", "a1", "a2", "a3"
    ); */

    ((void (*)(uint32_t data_offset, uint32_t data_length, uint32_t entry_pt, 
        uint32_t load_offset)) (loader_addr))
        (load_offset, mem_sz, hdr.entry, hdr.entry);

    return -1; /* something failed, badly */
}
