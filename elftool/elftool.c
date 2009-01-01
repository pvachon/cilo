/**
 * ELFTool: A tool for analyzing ELF32 files and a test case for the ELF
 * structures in elf.h for CiscoLoad
 * (c) 2008 Philippe Vachon <philippe@cowpig.ca>
 *
 * Licensed under the GNU General Public License v2
 * See COPYING in the root directory of the CiscoLoad source distribution for
 * more information
 */
#include <elf.h>
#include <stdio.h>
#include <malloc.h>

/**
 * swap functions - convert a header structure's endianess to the native
 * machine's endianess. Determined by the contents of the e_ident bytes in
 * the ELF file header.
 * @param hdr header structure to be translated
 */
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

void swap_elf32_section_header(struct elf32_section_header *shdr)
{
    shdr->name = SWAP_32(shdr->name);
    shdr->type = SWAP_32(shdr->type);
    shdr->flags = SWAP_32(shdr->flags);
    shdr->addr = SWAP_32(shdr->addr);
    shdr->offset = SWAP_32(shdr->offset);
    shdr->size = SWAP_32(shdr->size);
    shdr->link = SWAP_32(shdr->link);
    shdr->info = SWAP_32(shdr->info);
    shdr->addralign = SWAP_32(shdr->addralign);
    shdr->entsize = SWAP_32(shdr->entsize);
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

static char *e_types[] = { 
    [ELF_TYPE_NONE] = "No specified file type",
    [ELF_TYPE_REL] = "Relocatable File",
    [ELF_TYPE_EXEC] = "Executable File",
    [ELF_TYPE_DYN] = "Dynamic Shared Object File",
    [ELF_TYPE_CORE] = "Core File"    
};

const char *file_type_to_string(uint16_t e_type)
{
    if (e_type <= 4) {
        return e_types[e_type];
    } else {
        return "Unknown or custom file type";
    }
}

static char *e_machines[] = {
    [ELF_MACH_NONE] = "Unknown/No Machine Type",
    [ELF_MACH_M32] = "AT&T WE 32100",
    [ELF_MACH_SPARC] = "Sun SPARC",
    [ELF_MACH_386] = "Intel i386 (80386)",
    [ELF_MACH_68K] = "Motorola 68000",
    [ELF_MACH_88K] = "Motorola 88000",
    [ELF_MACH_860] = "Intel i860 (80860)",
    [ELF_MACH_MIPS] = "MIPS R3000 Big-Endian",
    [0x9] = "IBM S/370",
    [ELF_MACH_MIPS_R4K_BE] = "MIPS R4000 Big-Endian (Deprecated)",
    [0xb] = "IBM RS/6000",
    [0xc] = "Unknown",
    [0xd] = "Unknown",
    [0xe] = "Unknown",
    [0xf] = "HP PA-RISC",
    [0x10] = "Unknown",
    [0x11] = "Fujitsu VPP500",
    [0x12] = "Sun SPARC V.8+",
    [0x13] = "Intel i960 (80960)",
    [0x14] = "Cisco 4500 or IBM PowerPC",
    [0x15] = "IBM PowerPC (64-bit)",
    [0x16] = "IBM S/390",
    [0x17] = "Unknown",
    [0x18] = "Unknown",
    [0x19] = "Cisco 7200 Series Router",
    [0x1a] = "Unknown",
    [0x1b] = "Unknown",
    [0x1c] = "Unknown",
    [0x1d] = "Unknown",
    [0x1e] = "Cisco 3620/3640 Router (MIPS, IDT R4700)",
    [0x1f] = "Unknown",
    [0x20] = "Unknown",
    [0x21] = "Unknown",
    [0x22] = "Unknown",
    [0x23] = "Unknown",
    [0x24] = "Cisco 12000 Router or NEC V800",
    [0x25] = "Fujitsu FR20",
    [0x26] = "TRW RH-32",
    [0x27] = "Motorola RCE",
    [0x28] = "ARM",
    [0x29] = "Digital Equipment Corporation Alpha",
    [0x2a] = "Hitachi SuperH (SH)",
    [0x2b] = "Cisco 2600 Series Router (PowerPC, MPC860) or SPARC V.9 64-bit",
    [0x2c] = "Siemens Tricore Embedded Processor",
    [0x2d] = "Argonaut RISC Core",
    [0x2e] = "Hitachi H8/300",
    [0x2f] = "Hitachi H8/300H",
    [0x30] = "Hitachi H8S",
    [0x31] = "Hitachi H8/500",
    [0x32] = "Intel Itanium (Merced)",
    [0x33] = "Cisco 1700 Series Router (PowerPC, MPC860) or Stanford MIPS-X",
    [0x34] = "Cisco 3660 Series Router (MIPS, R5000) or Motorola Coldfire",
    [0x35] = "Motorola M68HC12",
    [0x36] = "Fujitsu MMA Multimedia Accelerator",
    [0x37] = "Siemens PCP",
    [0x38] = "Sony nCPU Embedded RISC",
    [0x39] = "Denso NDR1 Microprocessor",
    [0x3a] = "Motorola Star*Core Digital Signal Processor",
    [0x3b] = "Toyota ME16 Microprocessor",
    [0x3c] = "STMicroelectronics ST100 Microprocessor",
    [0x3d] = "Advanced Logic Corp. TinyJ Embedded Family Microprocessor",
    [0x3e] = "AMD x86-64/Intel EM64-T",
    [0x3f] = "Sony DSP Processor",
    [0x40] = "Unknown",
    [0x41] = "Unknown",
    [0x42] = "Siemens FX66 Microcontroller",
    [0x43] = "STMicroelectronics ST9+ 8/16-bit Microcontroller",
    [0x44] = "STMicroelectronics ST7 8-bit Microcontroller",
    [0x45] = "Motorola MC68HC16 Microcontroller",
    [0x46] = "Motorola MC68HC11 Microcontroller",
    [0x47] = "Motorola MC68HC08 Microcontroller",
    [0x48] = "Motorola MC68HC05 Microcontroller",
    [0x49] = "Silicon Graphics SVx",
    [0x4a] = "STMicroelectronics ST19 8-bit Microcontroller",
    [0x4b] = "Digital Equipment Corporation VAX",
    [0x4c] = "AXIS Communications 32-bit CRIS Processor",
    [0x4d] = "Infineon Tech. 32-bit Embedded Javelin Processor",
    [0x4e] = "Element 14 64-bit DSP",
    [0x4f] = "LSI Logic 16-bit DSP",
    [0x50] = "MMIX - Knuth's Educational 64-bit CPU",
    [0x51] = "Harvard University Machine-Independent Object File",
    [0x52] = "SiTera Prism",
    [0x53] = "Atmel AVR 8-bit Microcontroller",
    [0x54] = "Fujitsu FR30",
    [0x55] = "Mitsubishi D10V",
    [0x56] = "Mitsubishi D30V",
    [0x57] = "NEC V850 CPU",
    [0x58] = "Mitsubishi M32R",
    [0x59] = "Matsushita MN10300",
    [0x5a] = "Matsushita MN10200",
    [0x5b] = "picoJava",
    [0x5c] = "OpenRISC 32-bit Embedded Processor",
    [0x5d] = "ARC Cores Tangent-A5",
    [0x5e] = "Tensilica Xtensa Architecture",
    [0x5f] = "Unknown",
    [0x60] = "Unknown",
    [0x61] = "Cisco 3725 Router (MIPS, RM7000)",
    [0x62] = "Unknown",
    [0x63] = "Unknown",
    [0x64] = "Unknown",
    [0x65] = "Unknown",
    [0x66] = "Cisco 2691 Router (MIPS, R52xx)",
    [0x67] = "Unknown",
    [0x68] = "Unknown",
    [0x69] = "Cisco 3745 Router (MIPS)"
};

/**
 * Convert a e_machine code to a human-readable string, if it's a machine
 * we are aware of. 0x0 through 0xa are specified as a part of the ELF
 * specification, anything else has been found through experimentation or
 * through other sources of such information.
 *
 * @param machine the e_machine code read from the ELF file.
 * @return a string containing the machine type name.
 */
const char *machine_id_to_string(uint16_t machine)
{
    if (machine <= 0x69)
        return e_machines[machine];
    else
        return "Unknown";
}

/**
 * Convert section header type (e_type) to a human-readable string
 * @param sh_type the section header type
 * @return string with the human-readable name of the section header type
 */ 
char *sh_type_to_string(unsigned int sh_type)
{
    switch (sh_type) {
    case 0:
        return "SHT_NULL";
        break;
    case 1:
        return "SHT_PROGBITS";
        break;
    case 2:
        return "SHT_SYMTAB";
        break;
    case 3:
        return "SHT_STRTAB";
        break;
    case 4:
        return "SHT_RELA";
        break;
    case 5:
        return "SHT_HASH";
        break;
    case 6:
        return "SHT_DYNAMIC";
        break;
    case 7:
        return "SHT_NOTE";
        break;
    case 8:
        return "SHT_NOBITS";
        break;
    case 9:
        return "SHT_REL";
        break;
    case 10:
        return "SHT_SHLIB";
        break;
    case 11:
        return "SHT_DYNSYM";
        break;
    case 0x70000000:
        return "SHT_MIPS_LIBLIST";
        break;
    case 0x70000002:
        return "SHT_MIPS_CONFLICT";
        break;
    case 0x70000003:
        return "SHT_MIPS_GPTAB";
        break;
    case 0x70000004:
        return "SHT_MIPS_UCODE";
        break;
    case 0x70000005:
        return "SHT_MIPS_DEBUG";
        break;
    case 0x70000006:
        return "SHT_MIPS_REGINFO";
        break;
    default:
        return "SHT_CUSTOM";
    }
}

/**
 * Convert the segment/program header type number (e_type) to a human readable
 * string (based on the contents of the ELF specification)
 * @param type the e_type value read from the section header
 * @return a string containing the human-readable section type
 */
char *segment_type_to_string(uint32_t type)
{
    switch (type) {
    case ELF_PT_NULL:
        return "PT_NULL";
        break;
    case ELF_PT_LOAD:
        return "PT_LOAD";
        break;
    case ELF_PT_DYNAMIC:
        return "PT_DYNAMIC";
        break;
    case ELF_PT_INTERP:
        return "PT_INTERP";
        break;
    case ELF_PT_NOTE:
        return "PT_NOTE";
        break;
    case ELF_PT_SHLIB:
        return "PT_SHLIB";
        break;
    case ELF_PT_PHDR:
        return "PT_PHDR";
        break;
    case 0x70000000:
        return "PT_MIPS_REGINFO";
        break;
    default:
        return "PT_CUSTOM";
        break;
    }
}

int main(const int argc, const char *argv[])
{
    FILE *fp;
    uint8_t magic[4];
    struct elf32_header hdr;
    int swap = 0;

    printf("ELFTool - a simple tool for viewing ELF file structures.\n");
    printf("(c) 2008 Philippe Vachon <philippe@cowpig.ca>\n\n");

    if (argc < 2) {
        printf("A file must be specified for analysis.\n");
        return -1;
    }

    if ((fp = fopen(argv[1], "rb")) == NULL) {
        printf("Unable to open file %s. Aborting.\n",argv[1]); 
        return -1;
    }

    fread(magic, 1, 4, fp);

    if (magic[0] == 'M' && magic[1] == 'Z' && magic[2] == 'I' &&
        magic[3] == 'P')
    {
        printf("Found MZIP file. This tool is unable to analyze MZIP files.\n");
        fclose(fp);
        return -1;
    }
    else if (magic[0] != ELF_MAGIC_1 || magic[1] != ELF_MAGIC_2 ||
        magic[2] != ELF_MAGIC_3 || magic[3] != ELF_MAGIC_4)
    {
        printf("ELF magic number not found. Aborting.\n");
        printf("Magic found: 0x%08x.\n", magic);
        fclose(fp);
        return -1;
    }

    /* read in the ELF header fields */
    rewind(fp);
    fread(&hdr, 1, sizeof(struct elf32_header), fp);

    /* check endianess: */
    if (hdr.ident[ELF_INDEX_DATA] == ELF_DATA_MSB) {
        swap = 1;
        swap_elf32_header(&hdr);
    }

    /* print the ELF header structure: */
    printf("ELF File Header:\n");
    printf("================================================\n");

    printf("Ident bytes:\n");
    printf("\tClass: %d-bit\n", hdr.ident[ELF_INDEX_CLASS] == ELF_CLASS_64 ? 
        64 : 32);
    printf("\tData type: %s\n", hdr.ident[ELF_INDEX_DATA] == ELF_DATA_MSB ?
        "Big endian" : "Little endian");
    printf("\tVersion: %u\n\n", hdr.ident[ELF_INDEX_VERSION]);

    printf("ELF Object Type: %s (%u)\n", file_type_to_string(hdr.type), hdr.type);

    printf("Machine ID: %#4x\n\t%s\n", hdr.machine, machine_id_to_string(hdr.machine));
    printf("Version: 0x%08x\n", hdr.version);
    printf("Entry point: 0x%08x\n", hdr.entry);
    printf("Program header offset: 0x%08x\n", hdr.phoff);
    printf("Section header offset: 0x%08x\n", hdr.shoff);
    printf("Flags: 0x%08x\n", hdr.flags);
    printf("ELF Header Size: %u bytes\n", hdr.ehsize);
    printf("Program Header Entry Size: %u bytes\n", hdr.phentsize);
    printf("Program Header Count: %u\n", hdr.phnum);
    printf("Section header Entry Size: %u bytes\n", hdr.shentsize);
    printf("Section Header Count: %u\n", hdr.shnum);
    printf("String table entry index: %u\n\n", hdr.shstrndx);

    char *strtabl = NULL;
    if (hdr.shstrndx >= hdr.shnum) {
        printf("Warning: String table specified in header is greater than "
            "total number of \nsections.\n\n");
    }
    else
    {
        /* retrieve string table */
        struct elf32_section_header strtab;
        fseek(fp, hdr.shoff + (sizeof(struct elf32_section_header) * 
            hdr.shstrndx), SEEK_SET);

        fread(&strtab, sizeof(struct elf32_section_header), 1, fp);

        if (swap)
            swap_elf32_section_header(&strtab);
    
        /* check the sanity of the STRTAB section header */
        if (strtab.type != ELF_SHT_STRTAB && strtab.type!= ELF_SHT_NULL) {
            printf("Warning: String table specified in header is not a "
                "SHT_STRTAB!\n\n");
        } else if (strtab.type != ELF_SHT_NULL) {
            fseek(fp, strtab.offset, SEEK_SET);
            strtabl = (char *)malloc(strtab.size);
            fread(strtabl, strtab.size, 1, fp);
        }
    }



    /* now start looking at the ELF sections */
    fseek(fp, hdr.shoff, SEEK_SET);

    printf("Section Header Entries: \n");
    printf("================================================\n");

    int i;
    struct elf32_section_header shdr;
    for (i = 0; i < hdr.shnum; i++) {
        fread(&shdr.name, 1, 4, fp);    
        fread(&shdr.type, 1, 4, fp);    
        fread(&shdr.flags, 1, 4, fp);    
        fread(&shdr.addr, 1, 4, fp);
        fread(&shdr.offset, 1, 4, fp);    

        fread(&shdr.size, 1, 4, fp);    
        fread(&shdr.link, 1, 4, fp);    
        fread(&shdr.info, 1, 4, fp);    
        fread(&shdr.addralign, 1, 4, fp);    
        fread(&shdr.entsize, 1, 4, fp);    

        if (swap) {
            swap_elf32_section_header(&shdr);
        }

        printf("Section %d\n", i);
        printf("\tName: %s [%u]\n", 
            strtabl == NULL ? "unknown" : 
            strtabl[shdr.name] == '\0' ? "null" : &strtabl[shdr.name],
            shdr.name);
        printf("\tType: %s (0x%08x)\n", sh_type_to_string(shdr.type),
            shdr.type);
        printf("\tFlags: 0x%08x\n", shdr.flags);
        printf("\t\t%c%c%c\n", ELF_SHF_ALLOCD(shdr.flags) ? 'A' : '-',
            ELF_SHF_WRITABLE(shdr.flags) ? 'w' : '-',
            ELF_SHF_EXECUTABLE(shdr.flags) ? 'x' : '-');
        printf("\tAddress: 0x%08x\n", shdr.addr);
        printf("\tOffset: %u bytes\n", shdr.offset);
        printf("\tSize: %u bytes\n", shdr.size);
        printf("\tLink: 0x%08x\n", shdr.link);
        printf("\tAdditional Info: 0x%08x\n", shdr.info);
        printf("\tAddress Alignment: 0x%08x\n", shdr.addralign);
        printf("\tPer-Entry Size: %u bytes\n\n", shdr.entsize);
    }

    /* No program headers present; exit. */
    if (hdr.phnum == 0) {
        return 0;
    }


    fseek(fp, hdr.phoff, SEEK_SET);
    printf("\nProgram Header Entries\n");
    printf("================================================\n");
    struct elf32_phdr phdr;
    for (i = 0; i < hdr.phnum; i++) {
        fread(&phdr, 1, sizeof(struct elf32_phdr), fp);
        if (swap) {
            swap_elf32_program_header(&phdr);
        }
        printf("Segment %d\n", i);
        printf("\tType: %s (0x%08x)\n", segment_type_to_string(phdr.type), 
            phdr.type);
        printf("\tOffset: 0x%08x\n", phdr.offset);
        printf("\tVirtual Address: 0x%08x\n", phdr.vaddr);
        printf("\tPhysical Address: 0x%08x\n", phdr.paddr);
        printf("\tSize in File: %u bytes\n", phdr.filesz);
        printf("\tSize in Memory: %u bytes\n", phdr.memsz);
        printf("\tFlags: 0x%08x\n", phdr.flags);
        printf("\tAlignment: 0x%08x\n\n", phdr.align);
    }

    fclose(fp);

    if (strtabl) free(strtabl);

    return 0;
}
