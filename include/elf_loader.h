#ifndef _ELF_LOADER_H
#define _ELF_LOADER_H

#include <types.h>
#include <ciloio.h>

void load_elf32_file(struct file *fp, char *cmd_line);
void load_elf64_file(struct file *fp, char *cmd_line);

#endif /* _ELF_LOADER_H */
