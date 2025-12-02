#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "sockettome.h"
#include "chat_server.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"


void *client_thread(void *arg) {
    int n, done;
    char clean_line[1000], line[1000], name[1000];
    char *room_name, *message;
    FILE *fin, *fout;
    JRB tmp, tmp2;
    Client *c;
    Room *r;
    User *u;

    c = (Client *) arg;
    fin = fdopen(c->fd, "r");
    fout = fdopen(c->fd, "w");
    message = (char *) malloc(sizeof(char) * 1000);
    fprintf(fout, "Chat Rooms:\n\n");
    fflush(fout);
    
    jrb_traverse(tmp, c->rooms) {
        room_name = (char *) tmp->key.s;
        r = (Room *) tmp->val.v;
        fprintf(fout, "%s:", room_name);
        pthread_mutex_lock(r->lock);
        jrb_traverse(tmp2, r->members) {
            u = (User *) tmp2->val.v;
            fprintf(fout, " %s", u->name);
        }
        pthread_mutex_unlock(r->lock);
        fprintf(fout, "\n");
        fflush(fout);
    }
    fprintf(fout, "\nEnter your chat name (no spaces):\n");
    fflush(fout);
    done = 0;
    while(!done) {
        if(fgets(line, 1000, fin) == NULL) {
            close(c->fd);
            fclose(fin);
            fclose(fout);
            free(c);
            return NULL;
        }
        if (sscanf(line, "%s\n", clean_line) == 1) {
            strcpy(name, clean_line);
            done = 1;
        }

    }
    fprintf(fout, "Enter chat room:\n");
    fflush(fout);
    done = 0;
    while(!done) {
        if(fgets(line, 1000, fin) == NULL) {
            close(c->fd);
            fclose(fin);
            fclose(fout);
            free(c);
            return NULL;
        }

        if (sscanf(line, "%s\n", clean_line) == 1) {
            tmp = jrb_find_str(c->rooms, clean_line);
            

            if (tmp == NULL) {
                fprintf(fout, "Not a valid room.");
                fflush(fout);
            } 
            else {
                r = (Room *) tmp->val.v;
                u = (User *) malloc(sizeof(User));
                strcpy(u->name, name);
                u->fout = fout;
                pthread_mutex_lock(r->lock);
                jrb_insert_int(r->members, r->n, new_jval_v((void *) u));
                n = r->n;
                r->n++;
                strcpy(message, name);
                strcat(message, " has joined\n");
                dll_append(r->text, new_jval_s(strdup(message)));
                pthread_cond_signal(r->send);       
                pthread_mutex_unlock(r->lock);
                done = 1;
            }
        }

    
    }
    done = 0;
    while(!done && fgets(line, 1000, fin) != NULL) {
        pthread_mutex_lock(r->lock);
        strcpy(message, name);
        strcat(message, ": ");
        strcat(message, line);
        dll_append(r->text, new_jval_s(strdup(message)));
        pthread_cond_signal(r->send);
        pthread_mutex_unlock(r->lock);       
    }
    fclose(fin);
    pthread_mutex_lock(r->lock);
    close(c->fd);
    tmp = jrb_find_int(r->members, n);
    if(tmp != NULL) {
        //free(tmp->val.v);
        fclose(u->fout);
        jrb_delete_node(tmp);
    }
    free(u);
    

    strcpy(message, name);
    strcat(message, " has left\n");
    dll_append(r->text, new_jval_s(strdup(message)));
    pthread_cond_signal(r->send);
    pthread_mutex_unlock(r->lock);
    free(message);       
    free(c);
    
    return NULL;
}


void *room_thread(void *arg) {
    Room *r = (Room *) arg;
    User *u;
    JRB tmp;
    Dllist tmp2;
    Dllist l = new_dllist();
    int i;
    pthread_mutex_lock(r->lock);
    while(1) {
        pthread_cond_wait(r->send, r->lock);
        jrb_traverse(tmp, r->members) {
            u = (User *) tmp->val.v; 
            dll_traverse(tmp2, r->text) {
                if (fputs((char *) tmp2->val.s, u->fout) != EOF) {
                    if(fflush(u->fout) == EOF) {
                        i = (int) tmp->key.i;
                        dll_append(l, new_jval_i(i));
                    }
                }
                else {
                    i = (int) tmp->key.i;
                    dll_append(l, new_jval_i(i));
                }
            }
        }
        free_dllist(r->text);
        r->text = new_dllist();
        if(!dll_empty(l)) {
            dll_traverse(tmp2, l) {
                i = (int) tmp2->val.i;

                tmp = jrb_find_int(r->members, i);
                if(tmp != NULL) {
                    u = (User *) tmp->val.v;
                    jrb_delete_node(tmp);
                    fclose(u->fout);
                }
            }
            free_dllist(l);
            l = new_dllist();
        }
        
    }
  
}

int main(int argc, char **argv) {

    int sock, fd;
    JRB rooms;
    pthread_t tid, tid2;
    Client *c;
    Room *r;

    if (argc <= 2) {
        fprintf(stderr, "usage: <host> <port> <{chat rooms}> \n");
        exit(1);
    }

    sock = serve_socket(atoi(argv[2]));
    rooms =  make_jrb();
    for(int i = 3; i < argc; i++) {

        r = (Room *) malloc(sizeof(Room));
        r->members = make_jrb();
        r->text = new_dllist();
        r->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        r->send = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
        r->n = 0;
        pthread_mutex_init(r->lock, NULL);
        pthread_cond_init(r->send, NULL);
        jrb_insert_str(rooms, argv[i], new_jval_v((void *) r));
        pthread_create(&tid, NULL, room_thread, r);

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