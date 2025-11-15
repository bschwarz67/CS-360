all: chat_server

clean:
	rm -f chat_server 


LIBDIR = /home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/lib
LIBFDR_LIB = $(LIBDIR)/libfdr.a 
SOCKETTOME_LIB = sockettome.o
CFLAGS = -Wall -Wextra


chat_server: chat_server.c chat_server.h
	gcc $(CFLAGS) -o chat_server chat_server.c $(LIBFDR_LIB) $(SOCKETTOME_LIB) -lpthread
