/* MZIP file manipulation routines
 * (c) 2008 Philippe Vachon <philippe@cowpig.ca>
 * Licensed under the GNU General Public License 2.0 or later.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include <zip.h>

#include <mzip.h>

/**
 * Initialize an mzip_header struct. If hdr is null, returns a new
 * heap-allocated instance of an mzip_header
 * @param hdr The header to be initialized to default values. If this value
 *        is NULL, creates a new instance of struct mzip_header on the heap.
 * @return pointer to hdr; if hdr was NULL, contains a pointer to a new heap-
 *          allocated instance of a struct mzip_header
 */
struct mzip_header *mzip_initialize(struct mzip_header *hdr)
{
    struct mzip_header *mhdr = hdr;
    if (mhdr == NULL) {
        if(!(mhdr = (struct mzip_header *)malloc(MZIP_HDR_SIZE)))
        {
            /* unexpected error on allocating memory! */
            return NULL;
        }
    }

    memset((void *)mhdr, 0x0, MZIP_HDR_SIZE);

    mhdr->hdr_magic[0] = 'M';
    mhdr->hdr_magic[1] = 'Z';
    mhdr->hdr_magic[2] = 'I';
    mhdr->hdr_magic[3] = 'P';

    mhdr->hdr_version = 0x1u;

    return mhdr;
}

/**
 * Build an MZIP code segment; compresses code segment using ZIP
 * @param buffer Buffer containing uncompressed data
 * @param buf_size Size of buffer in bytes
 * @param out_seg Pointer to pointer to output buffer
 * @param seg_size size of compressed data stream (pass by reference)
 * @return 0 on success, -1 on error
 */
int mzip_codeseg_build(void *buffer, uint32_t buf_size, void **out_buf,
    uint32_t *seg_size)
{
    assert(buffer != NULL || out_buf != NULL || seg_size != NULL);

    char tempfile[] = "/tmp/mzipXXXXXX";
    int errorp = 0;

    /* get a temporary file */
    mktemp(tempfile);

    /* create the temporary ZIP archive */
    struct zip *archive = zip_open(tempfile, ZIP_CREATE | ZIP_EXCL, &errorp);

    if (archive == NULL)
        return -1;

    struct zip_source *source = zip_source_buffer(archive,  buffer, 
        buf_size, 0);

    if (source == NULL) {
        zip_close(archive);
        return -1;
    }

    if (zip_add(archive, "-", source) < 0) {
        zip_source_free(source);
        zip_close(archive);
        return -1;
    }

    zip_close(archive);

    /* now re-read the contents of the archive */
    FILE *fp = fopen(tempfile, "rb");
    if (fp == NULL) {
        return -1;
    }

    struct stat st;

    if (stat(tempfile, &st) == -1) {
        fclose(fp);
        return -1;
    }

    /* allocate a buffer to store the contents of the archive: */
    void *buf = malloc(st.st_size);
    if (buf == NULL) {
        fclose(fp);
        return -1;
    }

    fread(buf, st.st_size, 1, fp);

    *out_buf = buf;
    *seg_size = st.st_size;

    fclose(fp);
    
    return 0;
}

/**
 * Local method to calculate the CRC of a given memory buffer
 */
static uint16_t calculate_crc(void *buffer, uint32_t offset, uint32_t length,
    uint16_t remainder)
{
    uint16_t crc_table[256], ent;
    uint16_t crc = ~remainder;
    int i, j;

    for (i = 0; i < 256; i++) {
        ent = (uint16_t)i << 8;

        for (j = 0; j < 8; j++) {
            if (ent & 0x8000) ent = (ent << 1) ^ 0x1021;
            else ent <<= 1;
        }

        crc_table[i] = ent;
    }

    for (i = offset; i < length; i++) {
        crc = crc_table[((crc >> 8) ^ ((uint8_t *)buffer)[i]) & 0xff]
            ^ (crc << 8);
    }

    return ~crc;
}

/**
 * Calculate the MZIP CRC for the given buffer
 * @param buffer Buffer containing data to have the CRC computed
 * @param buf_size Size of buffer to have the CRC computed on
 * @param crc output of CRC of buffer. Should be initialized with remainder.
 * @return 0 on success, -1 on error
 */
int mzip_calculate_crc(struct mzip_header *hdr, void *buffer, 
    uint32_t buf_size, uint16_t *crc)
{
    assert(buffer != NULL && crc != NULL);

    uint16_t rem = calculate_crc((void *)hdr, 0x38, 0x70, 0);
    *crc = calculate_crc((void *)buffer, 0, buf_size, rem);

    return 0;
}

/**
 * Calculate the CRC of the mzip header and the remainder of the mzip
 * header.
 */ 
int mzip_calculate_hdr_crc(struct mzip_header *hdr, uint16_t *hdr_crc)
{
    assert(hdr != NULL);
    assert(hdr_crc != NULL); 

    *hdr_crc = calculate_crc((void *)hdr, 0, 0x36, 0);

    return 0;
}

/**
 * Write an MZIP header to the given file pointer
 * @param fp File pointer for the file to write the header out to
 * @param hdr MZIP header to be written out
 * @return 0 on success, -1 on error
 */
int mzip_write_header(FILE *fp, struct mzip_header *hdr)
{
    assert(hdr != NULL && fp != NULL);

    /* TODO: better sanity checks */
    rewind(fp);
    fwrite((void *)hdr, MZIP_HDR_SIZE, 1, fp);

    return 0;
}

/**
 * Write out the MZIP code segment.
 * @param fp File pointer
 * @param buffer Code segment buffer to be written out
 * @param buf_size Size of code segment buffer
 * @return 0 on success, -1 on error
 */
int mzip_write_codeseg(FILE *fp, void *buffer, uint32_t buf_size)
{
    assert(fp != NULL && buffer != NULL);    

    /* TODO: better sanity checks */
    
    fseek(fp, MZIP_HDR_SIZE, SEEK_SET);
    fwrite(buffer, buf_size, 1, fp);

    return 0;
}

/**
 * Write MZIP footer tuples
 * @param fp file handle for output file
 * @param footer the footer tuples
 * @param num_entries the number of footer tuple intries
 * @return 0 on success, -1 on failure
 */
int mzip_write_footer(FILE *fp, char **footer, uint32_t num_entries)
{
    int length, i;
    fseek(fp, 0, SEEK_END);

    for (i = 0; i < num_entries; i++) {
        length = strlen(footer[i]);
        fwrite(footer[i], length, 1, fp);
    }

    return 0;
}

/**
 * Write a full MZIP file out
 * @param fp file handle for output file
 * @param hdr MZIP file header
 * @param buffer the code segment for the MZIP file
 * @param buf_size size of code segment buffer
 * @param footer footer tuple values
 * @param num_entries number of footer tuple values
 */
int mzip_write(FILE *fp, struct mzip_header *hdr, void *buffer, 
    uint32_t buf_size, char **footer, uint32_t num_entries)
{
    assert(fp != NULL && hdr != NULL && buffer != NULL && footer != NULL);

    mzip_write_header(fp, hdr);
    mzip_write_codeseg(fp, buffer, buf_size);
    mzip_write_footer(fp, footer, num_entries);

    return 0;
}

/**
 * Print out contents of a struct mzip_hdr
 * @param hdr Pointer to struct mzip_hdr instance
 */

void mzip_print_header(struct mzip_header *hdr)
{
    printf("Magic: %c%c%c%c\n", hdr->hdr_magic[0], hdr->hdr_magic[1], 
        hdr->hdr_magic[2], hdr->hdr_magic[3]);
    printf("Version: %08x\n", hdr->hdr_version);
    printf("Entry Point: %08x\n", hdr->hdr_entrypt);
    printf("Flags 1: %08x\n", hdr->hdr_flags1);
    printf("Flags 2: %08x\n", hdr->hdr_flags2);
    printf("CRC Code Segment: %04x\n", hdr->hdr_crc_code);
    printf("CRC Header: %04x\n", hdr->hdr_crc_header);
    printf("Header Size: %08x\n", hdr->hdr_header_size);
    printf("Loader Address: %08x\n", hdr->hdr_loader_addr);
    printf("Flags 3: %08x\n", hdr->hdr_flags3);
    printf("Code size (packed): %08x\n", hdr->hdr_code_packed_size);
    printf("Code size (unpacked): %08x\n", hdr->hdr_code_unpacked_size);
    printf("Memory Image Size: %08x\n", hdr->hdr_memory_image_size);
}
