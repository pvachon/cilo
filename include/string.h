#ifndef _STRING_H
#define _STRING_H

#include <types.h>

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, uint32_t n);

char *strcpy(char *dest, const char *src);

char *strncpy(char *dest, const char *src, uint32_t n);

uint32_t strlen(char *s);

int memcpy(void *dst, const void *src, int n);

const char *strchr(const char *s, int c);

#endif /* _STRING_H */
