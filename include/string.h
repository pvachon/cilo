#ifndef _STRING_H
#define _STRING_H

#define NULL 0

inline int strcmp(const char *s1, const char *s2)
{
    while (*s1 == *s2 && *s1 != '\0' && *s2 != '\0') {
        s1++;
        s2++;
    }

    if (*s1 != *s2) return -1;

    return 0;
}

inline int strncmp(const char *s1, const char *s2, uint32_t n)
{
    int i;
    for (i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return -1;
        if (s1[i] == s2[i] && s1[i] == '\0') break;
    }

    return 0;
}

inline char *strcpy(char *dest, const char *src)
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

inline char *strncpy(char *dest, const char *src, uint32_t n)
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

inline uint32_t strlen(char *s)
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
inline int memcpy(void *dst, const void *src, int n)
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
inline const char *strchr(const char *s, int c)
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

#endif /* _STRING_H */
