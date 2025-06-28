CC = gcc 
LIBDIR = /home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/lib
CFLAGS = -Wall -Wextra -o
LIBS = $(LIBDIR)/libfdr.a 

EXECUTABLES = bin/fakemake

all: $(EXECUTABLES)

bin/fakemake: src/fakemake.c
	$(CC) $(CFLAGS) bin/fakemake src/fakemake.c $(LIBS) -lm

clean:
	rm core $(EXECUTABLES) *.o