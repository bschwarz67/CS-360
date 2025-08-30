all: bonding

clean:
	rm -f bonding 


LIBDIR = /home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/lib
LIBS = $(LIBDIR)/libfdr.a 
CFLAGS = -Wall -Wextra

bonding: bonding.c bonding.h bonding-driver.c
	gcc $(CFLAGS) -o bonding $(INCLUDE) bonding.c bonding-driver.c $(LIBS) -lpthread