CC = gcc 
LIBDIR = /home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/lib
LIBS = $(LIBDIR)/libfdr.a 

EXECUTABLES = bin/tarx

all: $(EXECUTABLES)

bin/tarx: src/tarx.c
	$(CC) -o bin/tarx src/tarx.c $(LIBS)

clean:
	rm -f $(EXECUTABLES)
