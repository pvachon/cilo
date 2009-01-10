ifndef CROSS_COMPILE
CROSS_COMPILE=powerpc-elf-
endif

OBJECTS=start.o promlib.o platform.o platio.o

INCLUDE=-I../../include

all: ${OBJECTS}

.c.o:
	$(CC) ${CFLAGS} ${INCLUDE} -c $<

.S.o:
	$(CC) ${CFLAGS} ${INCLUDE} ${ASFLAGS} -c $<

clean:
	-rm -f *.o
