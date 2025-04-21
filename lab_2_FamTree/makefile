CC = gcc 
LIBDIR = /home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/lib
LIBS = $(LIBDIR)/libfdr.a 

EXECUTABLES = bin/famtree

all: $(EXECUTABLES)

bin/famtree: src/famtree.c
	$(CC) $(CFLAGS) -o bin/famtree src/famtree.c $(LIBS)

clean:
	rm core $(EXECUTABLES) *.o
