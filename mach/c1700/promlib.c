/**
 * low-level prom and I/O library for the Cisco 1700 series
 * (C) 2008 Philippe Vachon <philippe@cowpig.ca>
 * -----------------------------------------------------------
 * Licensed under the GNU General Public License v2.0. See
 * COPYING in the root directory of the source distribution for details.
 */

#include <promlib.h>
#include <types.h>
#include <asm/ppc_asm.h>

#define UART_BASE 0x68050000
#define UART_LSR 0x5


/* putc
 * output character c to console
 * @param c ASCII number for character
 */
void c_putc(const char c)
{
    while (!(*((char *)(UART_BASE + UART_LSR)) & 0x20));
    ((char *)UART_BASE)[0] = c;
}

/* puts - wrapper for putc
 * output the string pointed to by s
 * @param s String to be written to the console
 */
void c_puts(const char *s)
{
    while(*s != '\0') {
        c_putc(*(s++));
        if (*s == '\n') {
            c_putc('\r');
        }
    }
}

/* putsn - put a string of length n on the console
 * @param s string to be written
 * @param n length
 */
void c_putsn(const char *s, int n)
{
    int i = 0;
    while (*s != '\0' && i != n) {
        c_putc(*(s++));
        i++;
    }
}


/* getc - Syscall n
 * get one character of input from the console
 * @return ASCII code for character read from console
 */
char c_getc(void)
{
    char c;
    while (!(*((uint8_t *)(UART_BASE + UART_LSR)) & 0x01));
    c = ((uint8_t *)UART_BASE)[0];
    return c;
}

/* gets - wrapper for getc
 * reads up to n characters into buffer b
 * @param b Buffer to read characters into
 * @param n size of buffer
 * @return the number of characters read into b
 */
int c_gets(char *b, int n)
{
    int i = 0;
    do {
        b[i] = c_getc();
        c_putc(b[i]);
        i++;
        if (b[i - 1] == '\n' || b[i-1] == '\r') {
            break;
        }
        else if (b[i - 1] == 0x8 || b[i - 1] == 0x7f) {
            i-=2;
        }
    } while (i < n);

    b[i - 1] = '\0';

    return i;

}

/* c_version - get version string
 * @return pointer to version string
 */
char *c_verstr(void)
{
    return "Cisco 1700 Series Router";
}

/* memsz - get total memory size (in bytes)
 * @return the size of the memory installed in the router, in bytes
 */
int c_memsz(void)
{
    int m = 0;

    __asm__ (
        "li 3, 4\n"
        "sc\n"
        "mr %[memsz], 3\n"
    : [memsz]"=r"(m)
    : /* no inputs */
    : "r3","memory","cc"
    );


    return m;
}

/* timer - get number of ticks from timer
 * @return the value in the timer
 */
long c_timer(void)
{
    return 0;
}

/* String length with a maximum length allowed
 * @param s pointer to string
 * @param maxlen maximum length
 */
int c_strnlen(const char *s, int maxlen)
{
    int i = 0;
    if (!s) return 0;

    while (*(s++) != '\0' && i != maxlen) {
        i++;
    }

    return i;
}

/* baud - get console baud rate
 * @return boot console baud rate
 */
int c_baud(void)
{
    int b = 115200;

    __asm__ (
        "li 3, 62\n"
        "sc\n"
        "mr %[baud], 3\n"
    : [baud]"=r"(b)
    : /* no returns */
    : "r3", "memory", "cc"
    );

    return b;
}
