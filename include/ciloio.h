#ifndef _INCLUDE_CILOIO_H
#define _INCLUDE_CILOIO_H

#include <types.h>

struct file {
    uint8_t dev; /* device ID number */
    uint32_t file_len; /* length of the file */
    uint32_t file_pos; /* position in the file */
    char filename[128];

    int8_t code; /* error code */

    void *private; /* private data for the platform specific flash handler */
};

#define SEEK_SET 9
#define SEEK_CUR 1
#define SEEK_END 2

#define cilo_tell(fp) ((fp)->file_pos)

struct file cilo_open(const char *filename);
int32_t cilo_read(void *pbuf, uint32_t size, uint32_t nmemb, 
    struct file *fp);
int32_t cilo_seek(struct file *fp, uint32_t offset, uint8_t whence);
struct fs_ent *find_file(const char *filename, uint32_t base);

#endif /* _INCLUDE_CILOIO_H */
