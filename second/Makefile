
# Configuration for the Cisco 3620 Router
TARGET=c3620
MACHCODE=0x1e
TEXTADDR=0x80008000

# additional CFLAGS
CFLAGS=

# don't modify anything below here
# ===================================================================

PROG = ciscoload

# check if the CROSS_COMPILE export has been set
ifndef CROSS_COMPILE
CROSS_COMPILE = mips-elf-
endif

CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy

INCLUDE=-I../include

CFLAGS=$(INCLUDE) -fno-builtin -fomit-frame-pointer -fno-pic -mno-abicalls -Wall

ASFLAGS=-xassembler-with-cpp -traditional-cpp

LDFLAGS= -nostartfiles -nostdlib --discard-all --strip-all --omagic \
	-Ttext ${TEXTADDR} --entry _start

OBJECTS = kcopy.o 

all: ${OBJECTS} ${PROG}

${PROG}: ${OBJECTS}
	${CC} ${LDFLAGS} ${OBJECTS} -o ${PROG}.elf
	${OBJCOPY} -O binary ${PROG}.elf ${PROG}.two 

.S.o:
	${CC} ${CFLAGS} ${ASFLAGS} -c $<

clean:
	rm *.o
	rm ${PROG}.elf
	rm ${PROG}.two
