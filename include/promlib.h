#ifndef _PROMLIB_H
#define _PROMLIB_H

/* Syscall numbers -- move into v0 
 * A lot of these revolve around "best guesses" based on some reverse
 * engineering.
 */
#define PUTC    1
#define GETC    3
#define MEMSIZE 4
#define VERSION 10

#define TIMER   0

#define GETBAUD 62

/* Promlib Calls */
void c_putc(const char c);
void c_puts(const char *s);
char c_getc(void);
int c_gets(char *b, int n);
int c_memsz(void);
long c_timer(void);
int c_strnlen(const char *c, int maxlen);
char *c_verstr(void);
int c_baud(void);

#endif /* _PROMLIB_H */
