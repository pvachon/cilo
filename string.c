#include <string.h>
#include <types.h>

int strcmp(const char *s1, const char *s2)
{
    while (*s1 == *s2 && *s1 != '\0' && *s2 != '\0') {
        s1++;
        s2++;
    }

    if (*s1 != *s2) return -1;

    return 0;
}

int strncmp(const char *s1, const char *s2, uint32_t n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return -1;
        if (s1[i] == s2[i] && s1[i] == '\0') break;
    }

    return 0;
}

char *strcpy(char *dest, const char *src)
{
    if (!dest || !src) {
        return NULL;
    }

    while (*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';

    return dest;
}

char *strncpy(char *dest, const char *src, uint32_t n)
{
    int i;
    int t = 0;
    if (!dest || !src || n == 0) {
        return NULL;
    }

    for (i = 0; i < n; i++) {
        if (!t) dest[i] = src[i];
        else dest[i] = '\0';

        if (dest[i] == '\0') t = 1;
    }

    return dest;
}

uint32_t strlen(const char *s)
{
    int i = 0;

    while (*s != '\0') {
        i++; s++;
    }

    return i;
}

/**
 * Copy n bytes from src to dst
 * @param dst destination buffer
 * @param src source buffer
 * @param n number of bytes to copy
 * @return number of bytes copied or value < 0 on error
 */
int memcpy(void *dst, const void *src, int n)
{
    int i = 0;
    if (!dst || !src) return -1;

    for (i = 0; i < n; i++) {
        ((char *)dst)[i] = ((char *)src)[i];
    }

    return i;
}

/**
 * strchr
 */
const char *strchr(const char *s, int c)
{
    const char *ptr = s;

    if (*ptr == (char)c) return ptr;

    while (*ptr != '\0') {
        if (*(++ptr) == (char)c) {
            return ptr;
        }
    }

    return NULL;
}

const char *strstr(const char *haystack, const char *needle)
{
    int npos = 0; /* position within needle */
    int nlen = strlen(needle);
    char *start;

    while (*haystack != '\0') {
        if (npos == 0) start = haystack; 

        if (needle[npos] == *(haystack++)) npos++;
        else npos = 0;

        if (npos == nlen - 1) return start; 
    }

    return NULL;
}
