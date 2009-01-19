#ifndef _INCLUDE_LZMA_LOADER_H
#define _INCLUDE_LZMA_LOADER_H

#include <types.h>
#include <ciloio.h>

void load_lzma(struct file *fp, uint32_t load_address, char *cmd_line);

#endif /* _INCLUDE_LZMA_LOADER_H */
