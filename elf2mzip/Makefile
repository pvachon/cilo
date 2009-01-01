CC = gcc
CFLAGS = -Wall -O2
INCLUDES = -I.

OBJECTS = elf2mzip.o mzip.o

COMPILE = $(CC) $(CFLAGS) $(INCLUDES)

LDFLAGS = -lzip

IMAGENAME = elf2mzip

all: elf2mzip

elf2mzip: $(OBJECTS)
	$(COMPILE) $(OBJECTS) $(LDFLAGS) -o $(IMAGENAME)

.c.o:
	$(COMPILE) -c $<

clean:
	-rm -rf $(IMAGENAME)
	-rm -rf $(OBJECTS)
