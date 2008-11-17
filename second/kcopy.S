/* Re-copy kernel image from given offset in a0 (length a1), and then
 * jump to entry point contained in a2. Copies data to location pointed
 * to in a3
 * Very naive.
 * ------------------------------------------------------------------
 * (c) 2008 Philippe Vachon <philippe@cowpig.ca>
 * Licensed under the GNU General Public License v3. See COPYING in the
 * source distribution for more details.
 */

#include <asm/regdef.h>
#include <asm/asm.h>

EXPORT(_start)

LEAF(_start)
    .set noreorder

1:  lw s0, 0(a0)   # load byte from address pointed to in a0 
    sw s0, 0(a3)    # copy byte to address pointed to in a3

    addiu a0, 4     # next location to read from
    addiu a3, 4     # next location to write to

    bnez a1, 1b     # continue copying
     addi a1, -4     # subtract from remaining bytes to copy

    nop
    
    /* put the stack pointer up at top of memory */
    li a0, 4
    syscall
    nop

    li sp, 0x80000000
    add sp, sp, v0

    move a0, v0
    li a1, 0x80000000
    li a3, 0
    li v0, 0

    
    jr a2           # jump to kernel entry point
     nop

    END(_start)
