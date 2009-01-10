/* Platform specific operations for I/O for the cisco 1700 Series
 * (C) 2008 Philippe Vachon <philippe@cowpig.ca>
 * 
 * Licensed under the GNU General Public License v2.
 */
#include <types.h>
#include <mach/c1700/platio.h>
#include <ciloio.h>
#include <string.h>

#include <mach/c1700/platform.h>

/* find file in filesystem starting at base */
struct fs_ent *find_file(const char *filename, uint32_t base)
{
    /* Actual file offset */
    uint32_t offset = 0;

    struct fs_ent *f = (struct fs_ent *)(base + offset);

    /* iterate over files in flash */
    while (f->magic == FS_FILE_MAGIC) {
        if (!strncmp(f->filename, filename, 48)) {
            return f;
        }

        offset += sizeof(struct fs_ent) + f->length;
        f = (struct fs_ent *)(base + offset);
    }

    return NULL;
}

/**
 * Find a file within the platform-supported I/O devices
 * @param filename the file
 * @returns 0 on failure, device ID number on success
 */
uint8_t platio_find_file(const char *filename)
{
    if (find_file(filename, FLASH_BASE)) {
        return 1;
    }
    /* todo: add support for PCMCIA devices */

    return 0;
}

/**
 * Open a file.
 * @param fp File structure to hold file information
 * @param filename name of the file
 */
void platio_file_open(struct file *fp, const char *filename)
{
    struct fs_ent *ent = find_file(filename, FLASH_BASE);

    if (ent == NULL) {
        fp->code = 0;
        fp->private = NULL;
        return;
    }

    fp->dev = 1; /* TODO: add support for PCMCIA flash */

    fp->private = (void *)ent;

    fp->file_len = ent->length;
    fp->file_pos = 0;

    /* copy the filename */
    strncpy(fp->filename, ent->filename, 48);

    fp->code = 1;
    
}

/**
 * Read data from a given file
 * @param pbuf Buffer to read data into
 * @param size size of entity to be read
 * @param nmemb number of members to read
 * @param fp file information structure to read from.
 * @returns number of bytes read (should = size * nmemb for C3600)
 */
uint32_t platio_read(void *pbuf, uint32_t size, uint32_t nmemb, struct file *fp)
{
    /* calculate the effective offset of the data we want to read: */
    char *from = (char *)((uint32_t)(fp->private) + sizeof(struct fs_ent) + 
        fp->file_pos);

    memcpy(pbuf, from, size * nmemb);

    fp->file_pos += size * nmemb;

    return nmemb * size;

}
