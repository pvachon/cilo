CC = gcc
CFLAGS = -g -Wall # -O2
INCLUDES = -I.

OBJECTS = elf2img.o mzip.o

COMPILE = $(CC) $(CFLAGS) $(INCLUDES)

LDFLAGS = -lzip

IMAGENAME = elf2img

all: elf2img

elf2img: $(OBJECTS)
	$(COMPILE) $(OBJECTS) $(LDFLAGS) -o $(IMAGENAME)

.c.o:
	$(COMPILE) -c $<

clean:
	-rm -rf $(IMAGENAME)
	-rm -rf $(OBJECTS)
