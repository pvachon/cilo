#ifndef _INCLUDE_MACH_C1700_PLATIO
#define _INCLUDE_MACH_C1700_PLATIO

#include <types.h>
#include <ciloio.h>

/* a flash filesystem entry for the C1700 */
struct fs_ent {
    uint32_t magic;
    uint32_t length;
    /* todo: figure out exactly what these two fields contain */
    uint32_t crc32;
    uint32_t date;
    char filename[48];
};

void platio_file_open(struct file *fp, const char *filename);
uint32_t platio_read(void *pbuf, uint32_t size, uint32_t nmemb,
    struct file *fp);
uint8_t platio_find_file(const char *filename);

#define FS_FILE_MAGIC 0xbad00b1e

#endif /* _INCLUDE_MACH_C1700_PLATIO */
