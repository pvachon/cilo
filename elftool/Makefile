OBJECTS = elftool.o
INCLUDES = -I../include/
CFLAGS = $(INCLUDES)
PROG = elftool

all: elftool

elftool: $(OBJECTS)
	gcc $(OBJECTS) -o $(PROG)

.c.o:
	gcc $(CFLAGS) -c $<

clean:
	rm *.o
	rm $(PROG)
