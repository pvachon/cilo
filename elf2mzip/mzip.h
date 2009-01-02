#ifndef __INCLUDE_MZIP_H
#define __INCLUDE_MZIP_H

#include <types.h>
#include <stdio.h>

struct mzip_header {
    char hdr_magic[4];
    uint32_t hdr_version;
    uint32_t hdr_entrypt;
    uint32_t hdr_flags1;
    uint32_t hdr_flags2;
    uint32_t hdr_padding1[8];
    uint16_t hdr_crc_code;
    uint16_t hdr_crc_header;
    uint32_t hdr_header_size;
    uint32_t hdr_loader_addr;
    uint32_t hdr_flags3;
    uint32_t hdr_code_packed_size;
    uint32_t hdr_code_unpacked_size;
    uint32_t hdr_memory_image_size;
    uint32_t hdr_padding2[8];
};

#define MZIP_HDR_SIZE (sizeof(struct mzip_header))

/* TODO: define macros to set values of flags fields */

struct mzip_header *mzip_initialize(struct mzip_header *hdr);

void mzip_swap(struct mzip_header *hdr);

int mzip_codeseg_build(void *buffer, uint32_t buf_size, void **out_buf, 
    uint32_t *seg_size);

int mzip_calculate_crc(struct mzip_header *hdr, void *buffer, 
    uint32_t buf_size, uint16_t *crc);

int mzip_calculate_hdr_crc(struct mzip_header *hdr, uint16_t *hdr_crc);

int mzip_write_header(FILE *fp, struct mzip_header *hdr);

int mzip_write_codeseg(FILE *fp, void *buffer, uint32_t buf_size);

int mzip_write_footer(FILE *fp, char **footer, uint32_t num_entries);

int mzip_write(FILE *fp, struct mzip_header *hdr, void *buffer, 
    uint32_t buf_size, char **footer, uint32_t num_entries);

void mzip_print_header(struct mzip_header *hdr);

#endif /* __INCLUDE_MZIP_H */
