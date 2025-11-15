#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "sockettome.h"
#include "chat_server.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"

//addjrb for rooms/room struct

int main(int argc, char **argv)
{
    int sock;
    
    if (argc <= 1) {
        fprintf(stderr, "usage: <port> <{chat rooms}> \n");
        exit(1);
    }
    for(int i = 2; i < argc; i++) {
        printf("%s\n", argv[i]);

        Room *r = (Room *) malloc(sizeof(Room));
        r->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        r->send = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
        pthread_mutex_init(r->lock, NULL);
        pthread_cond_init(r->send, NULL);
    }

    
    
    sock = serve_socket(atoi(argv[1]));

    
    
    return 0;
}