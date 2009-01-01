/* Re-copy kernel image from given offset in a0 (length a1), and then
 * jump to entry point contained in a2. Copies data to location pointed
 * to in a3
 * void kload(memory_start, memory_length, entry_point, copy_start)
 * Very naive.
 * ------------------------------------------------------------------
 * (c) 2008 Philippe Vachon <philippe@cowpig.ca>
 * Licensed under the GNU General Public License v3. See COPYING in the
 * source distribution for more details.
 */

#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/asm.h>

#define KSEG0       0x80000000
#define CACHE_SIZE  0x4000

#define ICACHE_INVALIDATE               0x00
#define DCACHE_WRITEBACK_INVALIDATE     0x01

#define CACHE_LINE_LEN          0x4

EXPORT(_start)

LEAF(_start)
    .set noreorder
    .set mips3

    /* invalidate the caches */
    li k0, KSEG0
    addi k1, k0, CACHE_SIZE

2:  cache ICACHE_INVALIDATE, 0(k0)
    cache DCACHE_WRITEBACK_INVALIDATE, 0(k0)

    ble k0, k1, 2b
     addi k0, CACHE_LINE_LEN


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

    li sp, KSEG0
    add sp, sp, v0
    
    /* set SR(BEV) = 0 */
    mfc0 k0, CP0_STATUS
    nop 
    /* fix... me... please */
    li k1, 0xFFBFFFFF
    and k0, k1
    mtc0 k0, CP0_STATUS

    /* jump to the kernel, setting up the firmware args appropriately */
    move a0, v0
    li a1, KSEG0
    li a3, 0
    li v0, 0

    jr a2           # jump to kernel entry point
     nop

    END(_start)
