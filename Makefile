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

MACHDIR=mach/$(TARGET)

# command to prepare a binary
RAW=${OBJCOPY} --strip-unneeded --alt-machine-code ${MACHCODE}

INCLUDE=-Iinclude/ -Imach/${TARGET} -Iinclude/mach/${TARGET}

CFLAGS=$(INCLUDE) -fno-builtin -fomit-frame-pointer -fno-pic -mno-abicalls \
	-Wall

ASFLAGS=-xassembler-with-cpp -traditional-cpp

LDFLAGS=--omagic -nostartfiles -nostdlib --discard-all --strip-all \
	-Ttext ${TEXTADDR} --entry _start

OBJECTS=main.o printf.o elf_loader.o

LINKOBJ=${OBJECTS} $(MACHDIR)/promlib.o $(MACHDIR)/start.o #$(MACHDIR)/platform.o


THISFLAGS='LDFLAGS=$(LDFLAGS)' 'ASFLAGS=$(ASFLAGS)' \
	'CROSS_COMPILE=$(CROSS_COMPILE)' 'CFLAGS=$(CFLAGS)' 'CC=$(CC)'

all: ${OBJECTS} ${PROG}

${PROG}: sub ${OBJECTS}
	${CC} ${LDFLAGS} ${LINKOBJ} -o ${PROG}.elf
	${RAW} ${PROG}.elf ${PROG}.bin

.c.o:
	${CC} ${CFLAGS} -c $<

.S.o:
	${CC} ${CFLAGS} ${ASFLAGS} -c $<
	
sub:
	@for i in $(MACHDIR); do \
	echo "Making all in $$i..."; \
	(cd $$i; $(MAKE) $(MFLAGS) $(THISFLAGS) all); done
	(cd second/; $(MAKE) $(MFLAGS))

subclean:
	@for i in $(MACHDIR); do \
	echo "Cleaning all in $$i..."; \
	(cd $$i; $(MAKE) $(MFLAGS) clean); done
	(cd second/; $(MAKE) $(MFLAGS) clean)

clean: subclean
	-rm -f *.o
	-rm -f ${PROG}.elf
	-rm -f ${PROG}.bin
