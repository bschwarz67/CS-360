#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "sockettome.h"
#include "chat_server.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"


void *client_thread(void *arg) {
    int fd, done;
    char clean_line[300], line[300], name[300];
    char *room_name;
    FILE *fin, *fout;
    JRB tmp, tmp3;
    Dllist tmp2;
    Client *c;
    Room *r;
    User *u;
    //TODO remove User, close their streams once the user leaves, do memory clean up.

    c = (Client *) arg;
    fin = fdopen(c->fd, "r");
    fout = fdopen(c->fd, "w");
    printf("%d\n",c->fd);
    fprintf(fout, "Chat Rooms:");
    fprintf(fout, "\n");
    fprintf(fout, "\n");
    fflush(fout);
    
    jrb_traverse(tmp, c->rooms) {
        room_name = (char *) tmp->key.v;
        r = (Room *) tmp->val.v;
        fputs(room_name, fout);
        fprintf(fout, ":");
        jrb_traverse(tmp3, r->members) {
            u = (User *) tmp3->val.v;
            fprintf(fout, " ");
            fputs(u->name, fout);
        }
        fprintf(fout, "\n");
        fflush(fout);
    }

    fprintf(fout, "Enter your chat name (no spaces):");
    fprintf(fout, "\n");
    fflush(fout);
    done = 0;
    while(!done && fgets(line, 300, fin) != NULL) {

        if (sscanf(line, "%s\n", clean_line) == 1) {
            strcpy(name, clean_line);
            done = 1;
            printf("name %s\n", name);
        }
    }
    fprintf(fout, "Enter chat room:");
    fprintf(fout, "\n");
    fflush(fout);
    done = 0;
    while(!done && fgets(line, 300, fin) != NULL) {

        if (sscanf(line, "%s\n", clean_line) == 1) {
            printf("cleaned %s\n", clean_line);
            tmp = jrb_find_str(c->rooms, clean_line);
            

            if (tmp == NULL) {
                fprintf(fout, "Not a valid room.");
                fflush(fout);
            } 
            else {
                //broadast to all fd's in the room that a new user joined, add the fd to the room's fd, listen for input, put into the dlist then signal to room thread to broadcast
                r = (Room *) tmp->val.v;
                u = (User *) malloc(sizeof(User));
                strcpy(u->name, name);
                printf("i work here\n");
                //u->fout = new_jval_v((void *) fout);
                u->fout = fout;
                u->fin = fin;
                printf("her as well\n");
                jrb_insert_int(r->members, r->n, new_jval_v((void *) u));
                r->n++;
                printf("here\n");
                jrb_traverse(tmp, r->members) {
                    u = (User *) tmp->val.v; 
                    //fout = (FILE *) u->fout.v;
                    fprintf(u->fout, "%s has joined\n",name);
                    fflush(u->fout);
                }
                done = 1;
                printf("correct room entered, room found, threat created\n");
            }
        }

    
    }
    //in case of an interrupt, need to close all hanging fin/fout and remove the User from the JRB
    printf("exiting...\n");
}


void *room_thread(void *arg) {
    


}

int main(int argc, char **argv) {
    int sock, fd;
    JRB rooms, tmp;
    pthread_t tid, tid2;
    Client *c;
    Room *r;
    char *room_name;

    if (argc <= 1) {
        fprintf(stderr, "usage: <port> <{chat rooms}> \n");
        exit(1);
    }

    sock = serve_socket(atoi(argv[1]));
    rooms =  make_jrb();
    for(int i = 2; i < argc; i++) {
        printf("%s\n", argv[i]);

        Room *r = (Room *) malloc(sizeof(Room));
        r->members = make_jrb();
        r->text = new_dllist();
        r->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        r->send = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
        r->n = 0;
        pthread_mutex_init(r->lock, NULL);
        pthread_cond_init(r->send, NULL);
        jrb_insert_str(rooms, argv[i], new_jval_v((void *) r));
        pthread_create(&tid, NULL, room_thread, r);
        r->sock = sock;

    }


    while (1) {
        fd = accept_connection(sock);
        c = (Client *) malloc(sizeof(Client));
        c->fd = fd;
        c->rooms = rooms;
        pthread_create(&tid2, NULL, client_thread, c);

        
        
        

        
        
    }

    
    
    return 0;
}