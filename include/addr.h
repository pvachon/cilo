#ifndef _ADDR_H
#define _ADDR_H

/* Address conversion macros to simplify dealing with known addresses
 * of devices in the system. Applies to all MIPS devices
 */

/* 32-bit addresses */

#define KSEG0_TO_PHYS32(a) ((a) & 0x7FFFFFFFul)
#define KSEG1_TO_PHYS32(a) ((a) & 0x1FFFFFFFul)

#define PHYS_TO_KSEG032(a) ((a) + 0x80000000ul)
#define PHYS_TO_KSEG132(a) ((a) + 0xE0000000ul)

/* 64-bit addresses */

#define KSEG0_TO_PHYS64(a) ((a) & 0xFFFFFFFF7FFFFFFFull)
#define KSEG1_TO_PHYS64(a) ((a) & 0xFFFFFFFF1FFFFFFFull)

#define PHYS_TO_KSEG064(a) ((a) + 0x0000000080000000ull)
#define PHYS_TO_KSEG164(a) ((a) + 0x00000000E0000000ull)

#endif /* _ADDR_H */
