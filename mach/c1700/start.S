/* CILO entry point for the Cisco 1700 Series Routers
 * (C) 2009 Philippe Vachon <philippe@cowpig.ca>
 * Licensed under the GNU General Public License v2.0 or later. See 
 * COPYING in the root of the source distribution for more details.
 */

#include <asm/ppc_asm.h>

    .text
    .globl _start

_start:

    /* put a letter C to show we are alive */
    lis r26, 0x6805
putchar:  
    lbz r0, 5(r26)
    andi. r0, r0, 0x20
    beq putchar

    /* put the C */
    li  r3, 67 
    stb r3, 0(r26)

    /* jump to the C code */
    bl start_bootloader

    .end _start
    .size _start, .-_start
