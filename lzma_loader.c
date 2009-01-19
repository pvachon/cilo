/*
 * LZMA Compressed Kernel Loader
 * (c) 2009 Philippe Vachon <philippe@cowpig.ca>
 *
 * Licensed under the GNU General Public License v.2.
 * See COPYING in the root directory of this source distribution for more
 * details.
 */

#include <printf.h>
#include <promlib.h>
#include <lzma_loader.h>

#include <ciloio.h>

/* LZMA SDK */
#include <LzmaDecode.h>

struct private_data {
    ILzmaInCallback callback;
    uint8_t *buffer;
    struct file *fp;
    uint32_t total_read;
    uint32_t last;
};

int read_data(void *object, const uint8_t **buffer, uint32_t *size)
{
    struct private_data *pvt = (struct private_data *)object;

    if (cilo_tell(pvt->fp) > pvt->fp->file_len) {
        printf("FATAL: Attempt to read past end of file. Aborting.\n");
        return LZMA_RESULT_DATA_ERROR;
    }
    
    *size = pvt->fp->file_len - cilo_tell(pvt->fp);
    *size = *size > 512 ? 512 : *size;

    pvt->total_read += *size;

    uint32_t done = (pvt->total_read * 100)/pvt->fp->file_len;
    if (done % 10 == 0 && done != pvt->last) {
        printf("%d", done);
        pvt->last = done;
    } else if (done != pvt->last && done % 2 == 0) {
        printf(".");
        pvt->last = done;
    }

    cilo_read(pvt->buffer, *size, 1, pvt->fp);

    *buffer = pvt->buffer;

    return LZMA_RESULT_OK;
}

void load_lzma(struct file *fp, uint32_t load_address, char *cmd_line)
{
    CLzmaDecoderState state;
    uint8_t buffer[512];
    struct private_data pvt;
    uint8_t props[LZMA_PROPERTIES_SIZE];
    
    uint32_t out_size = 0;
    uint8_t out_size_read[4];

    /* seek to beginning of file */
    cilo_seek(fp, 0, SEEK_SET);

    /* Setup LZMA decoding properties */
    cilo_read(props, LZMA_PROPERTIES_SIZE, 1, fp);

    if (LzmaDecodeProperties(&state.Properties, props, LZMA_PROPERTIES_SIZE)
        != LZMA_RESULT_OK)
    {
        printf("Error while decoding LZMA properties. Aborting.\n");
        return;
    }

    /* read in out size */
    cilo_read(out_size_read, 1, 4, fp);

    out_size = out_size_read[0] | out_size_read[1] << 8 |
        out_size_read[2] << 16 | out_size_read[3] << 24;

    cilo_seek(fp, 4, SEEK_CUR);

    uint16_t probs[LzmaGetNumProbs(&state.Properties)];

    /* setup structs */
    pvt.callback.Read = read_data;
    pvt.buffer = buffer;
    pvt.fp = fp;
    pvt.total_read = 0;
    state.Probs = probs;
    pvt.last = 100;
    
    /* do the decoding */
    uint32_t out_processed = 0;

    int result = LzmaDecode(&state, (ILzmaInCallback *)&pvt, 
        (uint8_t *)load_address, out_size, &out_processed);

    if (result != LZMA_RESULT_OK) {
        printf("\nError in decoding LZMA-compressed kernel image. Aborting.\n");
        return;
    }

    /* kick into kernel: */
    printf("\nStarting kernel at 0x%016x.\n", load_address);
    ((void (*)(uint32_t mem_sz, char *cmd_line))(load_address))
        (c_memsz(), cmd_line);
}
