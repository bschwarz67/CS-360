#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "sockettome.h"
#include "chat_server.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"


void *client_thread(void *arg) {
    int n, done;
    char clean_line[300], line[300], name[300];
    char *room_name;
    FILE *fin, *fout;
    JRB tmp, tmp3;
    Dllist tmp2;
    Client *c;
    Room *r;
    User *u, *u2;

    c = (Client *) arg;
    fin = fdopen(c->fd, "r");
    fout = fdopen(c->fd, "w");
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
        }
    }
    fprintf(fout, "Enter chat room:");
    fprintf(fout, "\n");
    fflush(fout);
    done = 0;
    while(!done && fgets(line, 300, fin) != NULL) {

        if (sscanf(line, "%s\n", clean_line) == 1) {
            tmp = jrb_find_str(c->rooms, clean_line);
            

            if (tmp == NULL) {
                fprintf(fout, "Not a valid room.");
                fflush(fout);
            } 
            else {
                r = (Room *) tmp->val.v;
                u2 = (User *) malloc(sizeof(User));
                strcpy(u2->name, name);
                //u2->fout = new_jval_v((void *) fout);
                u2->fout = fout;
                u2->fin = fin;
                pthread_mutex_lock(r->lock);
                jrb_insert_int(r->members, r->n, new_jval_v((void *) u2));
                n = r->n;
                r->n++;
                
                jrb_traverse(tmp, r->members) {
                    u = (User *) tmp->val.v; 
                    //fout = (FILE *) u->fout.v;
                    fprintf(u->fout, "%s has joined\n",name);
                    fflush(u->fout);
                }
                pthread_mutex_unlock(r->lock);
                done = 1;
            }
        }

    
    }
    done = 0;
    while(!done && fgets(line, 300, fin) != NULL) {
        pthread_mutex_lock(r->lock);
        char message[304];
        strcpy(message, name);
        strcat(message, ": ");
        strcat(message, line);
        dll_append(r->text, new_jval_s(strdup(message)));
        pthread_cond_signal(r->send);       
        pthread_mutex_unlock(r->lock);
    }


    printf("%s exiting %s ......\n", u2->name, clean_line);
    close(c->fd);
    fclose(fin);
    fclose(fout);

    pthread_mutex_lock(r->lock);
    jrb_delete_node(jrb_find_int(r->members, n));
    free(u2);
    jrb_traverse(tmp, r->members) {
        u = (User *) tmp->val.v; 
        //fout = (FILE *) u->fout.v;
        fprintf(u->fout, "%s has left\n",name);
        fflush(u->fout);
    }
    pthread_mutex_unlock(r->lock);
    free(c);
}


void *room_thread(void *arg) {

    Room *r = (Room *) arg;
    User *u;
    JRB tmp;
    
    while(1) {
        pthread_mutex_lock(r->lock);
        pthread_cond_wait(r->send, r->lock);
        jrb_traverse(tmp, r->members) {
            u = (User *) tmp->val.v; 
            //fout = (FILE *) u->fout.v;
            fputs((char *) dll_last(r->text)->val.s, u->fout);
            fflush(u->fout);
        }
        pthread_mutex_unlock(r->lock);
    }
  

}

int main(int argc, char **argv) {
    int sock, fd;
    JRB rooms, tmp;
    pthread_t tid, tid2;
    Client *c;
    Room *r;
    char *room_name;

    if (argc <= 2) {
        fprintf(stderr, "usage: <host> <port> <{chat rooms}> \n");
        exit(1);
    }

    sock = serve_socket(atoi(argv[2]));
    rooms =  make_jrb();
    for(int i = 3; i < argc; i++) {

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