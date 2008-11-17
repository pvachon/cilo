ifndef CROSS_COMPILE
CROSS_COMPILE=mips-elf-
endif

OBJECTS=start.o promlib.o

INCLUDE=-I../../include

all: ${OBJECTS}

.c.o:
	$(CC) ${CFLAGS} ${INCLUDE} -c $<

.S.o:
	$(CC) ${CFLAGS} ${INCLUDE} ${ASFLAGS} -c $<

clean:
	-rm -f *.o
