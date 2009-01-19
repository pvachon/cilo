# Configuration for the Cisco 3620/3640 Routers
TARGET=c3600
MACHCODE=0x1e
TEXTADDR=0x80008000
LOADADDR=0x80028000
ifndef CROSS_COMPILE
CROSS_COMPILE=mips-elf-
endif
CFLAGS=-mno-abicalls
LDFLAGS=-Ttext ${TEXTADDR}

# Configuration for the Cisco 3660 Routers
# TARGET=c3600
# MACHCODE=0x34
# TEXTADDR=0x80008000
# LOADADDR=0x80028000
# ifndef CROSS_COMPILE
# CROSS_COMPILE=mips-elf-
# endif
# CFLAGS=-mno-abicalls
# LDFLAGS=-Ttext ${TEXTADDR}

# Configuration for the Cisco 1700 Series Routers
# TARGET=c1700
# MACHCODE=0x33
# TEXTADDR=0x80008000
# LOADADDR=0x80028000
# ifndef CROSS_COMPILE
# CROSS_COMPILE=powerpc-elf-
# endif
# LDFLAGS=-Ttext=${TEXTADDR}

# additional CFLAGS
CFLAGS+=

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

CFLAGS+=-fno-builtin -fomit-frame-pointer -fno-pic \
	-Wall -DLOADADDR=${LOADADDR}

ASFLAGS=-D__ASSEMBLY__-xassembler-with-cpp -traditional-cpp

LDFLAGS+=--omagic -nostartfiles -nostdlib --discard-all --strip-all \
	--entry _start

OBJECTS=string.o main.o ciloio.o printf.o elf_loader.o lzma_loader.o \
	LzmaDecode.o

LINKOBJ=${OBJECTS} $(MACHDIR)/promlib.o $(MACHDIR)/start.o $(MACHDIR)/platio.o\
	$(MACHDIR)/platform.o


THISFLAGS='LDFLAGS=$(LDFLAGS)' 'ASFLAGS=$(ASFLAGS)' \
	'CROSS_COMPILE=$(CROSS_COMPILE)' 'CFLAGS=$(CFLAGS)' 'CC=$(CC)'

all: ${OBJECTS} ${PROG}

${PROG}: sub ${OBJECTS}
	${CC} ${LDFLAGS} ${LINKOBJ} -o ${PROG}.elf
	${RAW} ${PROG}.elf ${PROG}.bin

.c.o:
	${CC} ${CFLAGS} $(INCLUDE) -c $<

.S.o:
	${CC} ${CFLAGS} $(INCLUDE) ${ASFLAGS} -c $<
	
sub:
	@for i in $(MACHDIR); do \
	echo "Making all in $$i..."; \
	(cd $$i; $(MAKE) $(MFLAGS) $(THISFLAGS) all); done

subclean:
	@for i in $(MACHDIR); do \
	echo "Cleaning all in $$i..."; \
	(cd $$i; $(MAKE) $(MFLAGS) clean); done

clean: subclean
	-rm -f *.o
	-rm -f ${PROG}.elf
	-rm -f ${PROG}.bin
