CC = gcc 
LIBDIR = /home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/lib
LIBS = $(LIBDIR)/libfdr.a 

EXECUTABLES = bin/tarc

all: $(EXECUTABLES)

bin/tarc: src/tarc.c
	$(CC) -o bin/tarc src/tarc.c $(LIBS)

clean:
	rm -f $(EXECUTABLES)
