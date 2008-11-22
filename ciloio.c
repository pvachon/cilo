/* (C) 2008 Philippe Vachon <philippe@cowpig.ca>
 * Licensed under the GNU General Public License v2
 */

#include <types.h>
#include <ciloio.h>

/* Platform-specific file I/O operations */
#include <platio.h>

struct file cilo_open(const char *filename) 
{
    struct file fp;
    if (!platio_find_file(filename)) {
        fp.code = -1;
        return fp;
    }

    platio_file_open(&fp, filename);

    return fp;
}

int32_t cilo_read(void *pbuf, uint32_t size, uint32_t nmemb, struct file *fp)
{
    return platio_read(pbuf, size, nmemb, fp);
}

int32_t cilo_seek(struct file *fp, uint32_t offset, uint8_t whence)
{
    switch (whence) {
    case SEEK_SET:
        if (offset > fp->file_len) fp->file_pos = fp->file_len;
        else fp->file_pos = offset;
        break;
    case SEEK_CUR:
        if (offset + fp->file_pos > fp->file_len) fp->file_pos = fp->file_len;
        else fp->file_pos += offset;
        break;
    case SEEK_END:
        /* unimplemented */
        break;
    default:
        return 0;
    }

    return 0;
}
