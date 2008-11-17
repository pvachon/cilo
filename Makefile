# Configuration for the Cisco 3620/3640 Routers
TARGET=c3600
MACHCODE=0x1e
TEXTADDR=0x80008000
ifndef CROSS_COMPILE
CROSS_COMPILE=mips-elf-
endif

# Configuration for the Cisco 3660 Routers
# TARGET=c3600
# MACHCODE=0x34
# TEXTADDR=0x80008000
# ifndef CROSS_COMPILE
# CROSS_COMPILE=mips-elf-
# endif

# Configuration for the Cisco 1700 Series Routers
# TARGET=c1700
# MACHINE=0x33
# TEXTADDR=0x80008000
# ifndef CROSS_COMPILE
# CROSS_COMPILE=powerpc-elf-
# endif

# additional CFLAGS
CFLAGS=

# don't modify anything below here
# ===================================================================

PROG=ciscoload

CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy

# command to prepare a binary
RAW=${OBJCOPY} --strip-unneeded --alt-machine-code ${MACHCODE}

INCLUDE=-Iinclude/ -Imach/${TARGET}

CFLAGS=$(INCLUDE) -fno-builtin -fomit-frame-pointer -fno-pic -mno-abicalls \
	-Wall

ASFLAGS=-xassembler-with-cpp -traditional-cpp

LDFLAGS=--omagic -nostartfiles -nostdlib --discard-all --strip-all \
	-Ttext ${TEXTADDR} --entry _start

OBJECTS=start.o main.o printf.o promlib.o elf_loader.o

all: ${OBJECTS} ${PROG}

${PROG}: ${OBJECTS}
	${CC} ${LDFLAGS} ${OBJECTS} -o ${PROG}.elf
	${RAW} ${PROG}.elf ${PROG}.bin

.c.o:
	${CC} ${CFLAGS} -c $<

.S.o:
	${CC} ${CFLAGS} ${ASFLAGS} -c $<

clean:
	-rm *.o
	-rm ${PROG}.elf
	-rm ${PROG}.bin
