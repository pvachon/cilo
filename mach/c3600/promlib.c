/**
 * PROM Library for Cisco Systems 3600 Series Routers
 * (C) 2008 Philippe Vachon <philippe@cowpig.ca>
 * -----------------------------------------------------------
 */

/**
 * Calling convention:
 * a0 - syscall number
 * a1 - arg1 ... etc
 * v0 - returned value from syscall
 */

#include <promlib.h>

/* putc - Syscall 1
 * output character c to console
 * @param c ASCII number for character
 */
void c_putc(const char c)
{
    asm ( ".set noreorder\n "
          "li $a0, %[syscall]\n"
         "lb $a1, (%[character])\n" 
          "syscall\n"
          "nop\n"
          ".set reorder\n"
    : /* no output */
    : [character] "r"(&c), [syscall] "g"(PUTC)
    : "a0", "a1"
    );
}

/* puts - wrapper for putc
 * output the string pointed to by s
 * @param s String to be written to the console
 */
void c_puts(const char *s)
{
    while(*s != '\0') {
        c_putc(*(s++));
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

    asm ( ".set noreorder\n "
           "li $a0, %[syscall]\n"
           "syscall\n"
           "nop\n"
           "move %[charout], $v0 \n"
           ".set reorder\n"
    : [charout] "=r" (c)
    : [syscall] "g" (GETC)
    : "a0","v0"
    );

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
        else if (b[i - 1] == 0x8) {
            i--;
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
    char *c;

    asm ( ".set noreorder\n"
          "li $a0, %[syscall]\n"
          "syscall\n"
          "nop\n"
          "move %[result], $v0\n"
          ".set reorder\n"
          : [result] "=r" (c)
          : [syscall] "g" (VERSION)
          : "a0", "v0"
    );

    return c;
}

/* memsz - get total memory size (in bytes)
 * @return the size of the memory installed in the router, in bytes
 */
int c_memsz(void)
{
    int r = 0;

    asm ( " .set noreorder \n"
            "li $a0, %[syscall]\n"
            "syscall\n"
            "nop\n"
            "move %[result], $v0\n"
            ".set reorder\n"
            : [result] "=r" (r)
            : [syscall] "g" (MEMSIZE)
            : "a0","v0"
    );

    return r;
}

/* timer - get number of ticks from timer
 * @return the value in the timer
 */
long c_timer(void)
{
    long t = 0;
    
    asm (" .set noreorder\n"
           "li $a0, %[syscall]\n"
           "syscall\n"
           "nop\n"
           "move %[result], $a0\n"
           ".set reorder\n"
         : [result]"=r"(t)
         : [syscall]"g"(TIMER)
         : "a0","v0"
    );

    return t;
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
    int b = 0;

    asm volatile (".set noreorder\n"
                  "li $a0, %[syscall]\n"
                  "syscall\n"
                  "nop\n"
                  "move %[result], $a0\n"
                  ".set reorder\n"
        : [result]"=r"(b)
        : [syscall]"g"(GETBAUD)
        : "a0", "v0"
    );
}
