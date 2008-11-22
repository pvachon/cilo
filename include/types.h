#ifndef _TYPES_H
#define _TYPES_H

/* define some convenience types */
#define NULL 0

typedef unsigned char uint8_t;
typedef char int8_t;

typedef unsigned short uint16_t;
typedef short int16_t;

typedef unsigned int uint32_t;
typedef int int32_t;

typedef unsigned long uint64_t;
typedef long int64_t;

/* endianess changes */
#define SWAP_32(x) \
    ((uint32_t)( \
        (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) | \
        (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) | \
        (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) | \
        (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24) ))

#define SWAP_16(x) \
    ((uint16_t) (\
        (((uint16_t)(x) & (uint16_t)0x00ff) << 8) | \
        (((uint16_t)(x) & (uint16_t)0xff00) >> 8)))


#endif /* _TYPES_H */
