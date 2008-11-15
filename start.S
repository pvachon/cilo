/* Initial entry point for ciscoboot. Sets up some stack at 0x80008000
 * and jumps to main
 */

#include <asm/regdef.h>
#include <asm/asm.h>

EXPORT(_start)

.extern start_bootloader 

LEAF(_start)
    .set noreorder
    li sp, 0x80008000
    
    /* Save return address */
    sw ra, -4(sp)

    /* print out a letter C */
    li a0, 1 
    li a1, 67 
    syscall
    nop

    /* get the total amount of RAM */
    li a0, 4
    syscall
    nop

    /* set the stack at the top of RAM */
    li sp, 0x80000000
    add sp, sp, v0

    /* save return address*/
    /*sw ra, -4(sp)

    addi sp, sp, -4 */

    /* start bootloader */
    jal start_bootloader
    nop

    /* bootloader failed for some reason: */
    li sp, 0x80008000

    /* re-load return address to ROM: */
    ld ra, -4(sp)

    jr ra

    .set reorder
    END(_start)
