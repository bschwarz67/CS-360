CC = gcc 
LIBDIR = /home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/lib
LIBS = $(LIBDIR)/libfdr.a 
CFLAGS = -Wall -Wextra

EXECUTABLES = bin/jshell

all: $(EXECUTABLES)

bin/jshell: src/jshell.c
	$(CC) $(CFLAGS) -o bin/jshell src/jshell.c $(LIBS)

clean:
	rm core $(EXECUTABLES) *.o