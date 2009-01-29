#ifndef _INCLUDE_MACH_C7200_PLATIO
#define _INCLUDE_MACH_C7200_PLATIO

#include <types.h>
#include <ciloio.h>

/* a flash filesystem entry for the C7200 */
struct fs_ent {
    uint32_t magic;	/* C7200 bootflash filesystem magi number */
    uint32_t fileno;	/* number of the file */
    char filename[64];	/* 64bytes filename string*/
    uint32_t length;	/* size of the file in bytes */
    uint32_t seek;	/* address of the next fs_ent from FLASH_BASE (0xBA000000) and not FLASHFS_BASE (0xBA040000) */
    uint32_t crc32;	/* CRC32 value of the file */
    uint32_t type;	/* file type (ascii or binary etc.) */
    uint32_t date;	/* the file last modification time in UNIX time */
    /* todo: figure out exactly what these two fields contain */
    uint32_t sg01;	/* 0xffffffffh */
    uint32_t sg02;	/* 0xfffffff8h */
    uint32_t sg03;	/* 0xffffffffh */
    uint32_t sg04;	/* 0xffffffffh */
    uint32_t sg05;	/* 0xffffffffh */
    uint32_t sg06;	/* 0xffffffffh */
    uint32_t sg07;	/* 0xffffffffh */
    uint32_t sg08;	/* 0xffffffffh */
    uint32_t sg09;	/* 0xffffffffh */
};

void platio_file_open(struct file *fp, const char *filename);

uint32_t platio_read(void *pbuf, uint32_t size, uint32_t nmemb, struct file *fp);

uint8_t platio_find_file(const char *filename);

#define FS_FILE_MAGIC 0x07158805

#endif /* _INCLUDE_MACH_C7200_PLATIO */
