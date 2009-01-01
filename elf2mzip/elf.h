#ifndef _ELF_H
#define _ELF_H

#include <types.h>

/* ELF object file types */
#define ELF_TYPE_NONE   0 /* no file type */
#define ELF_TYPE_REL    1 /* relocatable file */
#define ELF_TYPE_EXEC   2 /* executable file */
#define ELF_TYPE_DYN    3 /* shared object file */
#define ELF_TYPE_CORE   4 /* core file */
#define ELF_TYPE_LOPROC 0xff00 /* cpu specific */
#define ELF_TYPE_HIPROC 0xffff

/* ELF machine types */
#define ELF_MACH_NONE         0 /* no machine type */
#define ELF_MACH_M32          1 /* AT&T WE 32100 */
#define ELF_MACH_SPARC        2 /* Sun SPARC */
#define ELF_MACH_386          3 /* Intel i386 */
#define ELF_MACH_68K          4 /* Motorola 68000 */
#define ELF_MACH_88K          5 /* Motorola 88000 */
#define ELF_MACH_860          7 /* Intel 80860 */
#define ELF_MACH_MIPS         8 /* MIPS RS3000 Big-Endian */
#define ELF_MACH_MIPS_R4K_BE 10 /* MIPS RS4000 Big-Endian */
/* 11-16 are reserved */

/* ELF Version */
#define ELF_VER_NONE 0 /* invalid version */
#define ELF_VER_CURRENT 1 /* Current ELF version */


/* ELF Header Structure */

#define ELF_IDENT_COUNT 16

struct elf32_header {
    uint8_t ident[ELF_IDENT_COUNT]; /* key fields */
    uint16_t type;  /* object file type */
    uint16_t machine; /* architecture */
    uint32_t version; /* object file version */
    uint32_t entry; /* entry point */
    uint32_t phoff; /* program header offset */
    uint32_t shoff; /* section header offset */
    uint32_t flags; /* ELF file flags */
    uint16_t ehsize; /* ELF header size */
    uint16_t phentsize; /* size of a program header entry */
    uint16_t phnum; /* number of entries in the program header */
    uint16_t shentsize; /* size of a section header entry */
    uint16_t shnum; /* number of section header entries */
    uint16_t shstrndx; /* index of string table entry in the section hdr */
};

/* ELF magic */
#define ELF_MAGIC_1 0x7f
#define ELF_MAGIC_2 0x45
#define ELF_MAGIC_3 0x4c
#define ELF_MAGIC_4 0x46

/* ELF class */
#define ELF_CLASS_NONE 0
#define ELF_CLASS_32   1
#define ELF_CLASS_64   2

/* ELF Data Encoding */
#define ELF_DATA_NONE 0 /* invalid */
#define ELF_DATA_LSB  1 /* Little endian */
#define ELF_DATA_MSB  2 /* big endian */

/* Offsets within the ident string */
#define ELF_INDEX_MAGIC0  0
#define ELF_INDEX_MAGIC1  1
#define ELF_INDEX_MAGIC2  2
#define ELF_INDEX_MAGIC3  3 
#define ELF_INDEX_CLASS   4 /* file class */
#define ELF_INDEX_DATA    5 /* data encoding */
#define ELF_INDEX_VERSION 6 /* file version */
#define ELF_INDEX_PADDING 7 /* start of padding */

/* Special Section Header Indexes */
#define ELF_SH_UNDEF          0 /* no section header present */
#define ELF_SH_LORESERVE 0xff00 /* lower-bound of reserved indices */
#define ELF_SH_LOPROC    0xff00 /* processor-specifc semantics low bound */
#define ELF_SH_HIPROC    0xff1f /* processor-specific semantics high bound */
#define ELF_SH_ABS       0xfff1 /* abs values for symbols in this section */
#define ELF_SH_COMMON    0xfff2 /* Common Block/unallocated extern vars */
#define ELF_SH_HIRESERVE 0xffff /* high value for reserved indices */

/* Section Header Structure */
struct elf32_section_header {
    uint32_t name; /* index of the string table entry for this section */
    uint32_t type; /* section type */
    uint32_t flags; /* section flags */
    uint32_t addr; /* section address if in actual memory image */
    uint32_t offset; /* byte offset from beginning of file to start of sect. */
    uint32_t size; /* size of section in bytes */
    uint32_t link; /* section header table index link */
    uint32_t info; /* extra information */
    uint32_t addralign; /* alignment constraints */
    uint32_t entsize; /* per-entry size */
};

/* Section Types */
#define ELF_SHT_NULL     0 /* inactive/no associated section */
#define ELF_SHT_PROGBITS 1 /* program-specific information */
#define ELF_SHT_SYMTAB   2 /* Symbol table */
#define ELF_SHT_STRTAB   3 /* string table */
#define ELF_SHT_RELA     4 /* Relocation entries with explicit addends */
#define ELF_SHT_HASH     5 /* symbol hash table */
#define ELF_SHT_DYNAMIC  6 /* dynamic linking information */
#define ELF_SHT_NOTE     7 /* notational marking */
#define ELF_SHT_NOBITS   8 /* no space in file, looks like progbits */
#define ELF_SHT_REL      9 /* relocation entires without explicit addends */
#define ELF_SHT_SHLIB   10 /* reserved; non-ABI conformant code */
#define ELF_SHT_DYNSYM  11 /* symbol table */
#define ELF_SHT_LOPROC  0x70000000 /* region reserved for CPU-specific info */
#define ELF_SHT_HIPROC  0x7fffffff
#define ELF_SHT_LOUSER  0x80000000 /* section reseverd for prog. info */
#define ELF_SHT_HIUSER  0xffffffff

/* Section header flags */
#define ELF_SHF_WRITE            0x1 /* Contains writable data */
#define ELF_SHF_ALLOC            0x2 /* occupies memory during exec */
#define ELF_SHF_EXECINSTR        0x4 /* contains executable instructions */
#define ELF_SHF_MASKPROC  0xf0000000 /* Processor-specific mask bits */

#define ELF_SHF_WRITABLE(x) ((x) & ELF_SHF_WRITE)
#define ELF_SHF_ALLOCD(x) ((x) & ELF_SHF_ALLOC)
#define ELF_SHF_EXECUTABLE(x) ((x) & ELF_SHF_EXECINSTR)

/* Symbol Table */
#define ELF_STN_UNDEF 0

/* ELF Spec, p. 4-22 */
struct elf32_sym_table_entry {
    uint32_t name; /* string table entry */
    uint32_t value; /* value of the symbol */
    uint32_t size; /* size of the symbol */
    uint8_t info; /* symbol's type and binding attribs */
    uint8_t other; /* no defined meaning */
    uint16_t shndx; /* associated section header table index */
};

#define ELF_ST_BIND(i) ((i) >> 4)
#define ELF_ST_TYPE(i) ((i) & 0xf)
#define ELF_ST_INFO(b, t) (((b)<<4) + ((t) & 0xf))

/* symbol binding (use with ELF_ST_BIND */
#define ELF_STB_LOCAL   0 /* not visible outside of this object file */
#define ELF_STB_GLOBAL  1 /* visible to all object files being combined */
#define ELF_STB_WEAK    2 /* Weak bound symbols (lower precedence than glbl */
#define ELF_STB_LOPROC 13 /* Processor specific semantics range */
#define ELF_STB_HIPROC 15

/* symbol type (use with ELF_ST_TYPE macro) */
#define ELF_STT_NOTYPE  0 /* type is not specified */
#define ELF_STT_OBJECT  1 /* symbol is associated with a data object */
#define ELF_STT_FUNC    2 /* symbol is associated with executable code */
#define ELF_STT_SECTION 3 /* symbol is associated with a section */
#define ELF_STT_FILE    4 /* file symbol type */
#define ELF_STT_LOPROC 13 /* processor specific semantics range */
#define ELF_STT_HIPROC 15

/* Relocation Entries */
struct elf32_rel {
    uint32_t offset; /* where to apply relocation action */
    uint32_t info;  /* information about the symbol */
};

struct elf32_rel_add {
    uint32_t offset; /* where to apply relocation action */
    uint32_t info;   /* information about the symbol */
    uint32_t addend; /* constant addend */
};

/* Info helper macros: */
#define ELF_REL_SYM(i) ((i) >> 8)
#define ELF_REL_TYPE(i) ((uint8_t)(i))
#define ELF_REL_INFO(s, t) (((s) << 8) + (unsigned char)(t))

/* Program Header/Segments */
struct elf32_phdr {
    uint32_t type; /* segment type */
    uint32_t offset; /* offset from beginning of file */
    uint32_t vaddr; /* virtual address of first byte of segment */
    uint32_t paddr; /* physical address of first byte of segment */
    uint32_t filesz; /* size in file of this segment */
    uint32_t memsz; /* Size of segment in memory image */
    uint32_t flags; /* segment flags */
    uint32_t align; /* alignment requirements for loading */
};

/* Segment Types */
#define ELF_PT_NULL    0 /* unused */
#define ELF_PT_LOAD    1   /* loadable segment */
#define ELF_PT_DYNAMIC 2 /* dynamic section */
#define ELF_PT_INTERP  3 /* null-terminate path to an invokable interpreter */
#define ELF_PT_NOTE    4 /* Auxiliary information */
#define ELF_PT_SHLIB   5 /* Shared Library? No ABI conformity required */
#define ELF_PT_PHDR    6 /* Program header table size */
#define ELF_PT_LOPROC  0x70000000 /* processor-specific values */
#define ELF_PT_HIPROC  0x7fffffff

#endif /* _ELF_H */
