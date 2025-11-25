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
    JRB tmp, tmp3;
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
        room_name = (char *) tmp->key.s;
        r = (Room *) tmp->val.v;
        fputs(room_name, fout);
        fprintf(fout, ":");
        pthread_mutex_lock(r->lock);
        jrb_traverse(tmp3, r->members) {
            u = (User *) tmp3->val.v;
            fprintf(fout, " ");
            fputs(u->name, fout);
        }
        pthread_mutex_unlock(r->lock);
        fprintf(fout, "\n");
        fflush(fout);
    }
    printf("1...\n");
    fprintf(fout, "\n");
    fprintf(fout, "Enter your chat name (no spaces):");
    fprintf(fout, "\n");
    fflush(fout);
    done = 0;
    while(!done) {
        if(fgets(line, 1000, fin) == NULL) {
            printf("exiting EOF reached ......\n");
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
    printf("2...\n");
    fprintf(fout, "Enter chat room:");
    fprintf(fout, "\n");
    fflush(fout);
    done = 0;
    while(!done) {
        if(fgets(line, 1000, fin) == NULL) {
            printf("exiting EOF reached ......\n");
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
                u2 = (User *) malloc(sizeof(User));
                strcpy(u2->name, name);
                //u2->fout = new_jval_v((void *) fout);
                u2->fout = fout;
                pthread_mutex_lock(r->lock);
                jrb_insert_int(r->members, r->n, new_jval_v((void *) u2));
                n = r->n;
                r->n++;

                //fout = (FILE *) u->fout.v;

                message = (char *) malloc(sizeof(char) * 1000);
                strcpy(message, name);
                strcat(message, " has joined\n");
                dll_append(r->text, new_jval_s(strdup(message)));
                free(message);
                pthread_cond_signal(r->send);       
                pthread_mutex_unlock(r->lock);
                done = 1;
            }
        }

    
    }
    printf("3...\n");
    done = 0;
    while(!done && fgets(line, 1000, fin) != NULL) {
        pthread_mutex_lock(r->lock);
        message = (char *) malloc(sizeof(char) * 1000);
        strcpy(message, name);
        strcat(message, ": ");
        strcat(message, line);
        dll_append(r->text, new_jval_s(strdup(message)));
        int k = pthread_cond_signal(r->send);
        int p = pthread_mutex_unlock(r->lock);       
        printf("client signalled the room %s with %d %d\n", message, k, p);
        free(message);
        
        

    }
    printf("%s exiting %s ......\n", u2->name, clean_line);

    fclose(fin);
    pthread_mutex_lock(r->lock);
    close(c->fd);
    //free the node User node for this client thread and free the memory occupied by the node, close the remaining stream
    tmp = jrb_find_int(r->members, n);
    if(tmp != NULL) {
        fclose(u2->fout);
        jrb_delete_node(tmp);
    }
    free(u2);

    printf("%s freed\n",name);


    //signal the room thread to broadcast the message that the user has left to all other users
    

    message = (char *) malloc(sizeof(char) * 1000);
    strcpy(message, name);
    strcat(message, " has left\n");
    dll_append(r->text, new_jval_s(strdup(message)));
    pthread_cond_signal(r->send);
    pthread_mutex_unlock(r->lock);
    printf("room signaled, client freed\n");
    free(message);       
    free(c);
    
    return NULL;
}


void *room_thread(void *arg) {
    printf("begin room thread\n");
    Room *r = (Room *) arg;
    User *u;
    JRB tmp;
    Dllist tmp2;
    Dllist l = new_dllist();
    int i;
    pthread_mutex_lock(r->lock);
    while(1) {
        printf("waiting....\n");
        pthread_cond_wait(r->send, r->lock);
        printf("signalled..\n");
        printf("room1..\n");

        jrb_traverse(tmp, r->members) {
            u = (User *) tmp->val.v; 
            //fout = (FILE *) u->fout.v;
            dll_traverse(tmp2, r->text) {
                if (fputs((char *) tmp2->val.s, u->fout) != EOF) {
                    if(fflush(u->fout) == EOF) {
                        //fclose(u->fout);
                        //jrb_delete_node(tmp);
                        //these is could be potentially be getting added twice, should only get added once
                        i = (int) tmp->key.i;
                        dll_append(l, new_jval_i(i));
                    }
                    else {
                        printf("%s\n",tmp2->val.s);
                    }
                }
                else {
                    //fclose(u->fout);
                    //jrb_delete_node(tmp);
                    i = (int) tmp->key.i;
                    dll_append(l, new_jval_i(i));
                }
            }
            
        }
        free_dllist(r->text);
        r->text = new_dllist();
        printf("room2..\n");
        if(!dll_empty(l)) {
            dll_traverse(tmp2, l) {
                printf("room5..\n");
                i = (int) tmp2->val.i;

                if(jrb_find_int(r->members, i) != NULL) {
                    u = (User *) jrb_find_int(r->members, i)->val.v;
                    jrb_delete_node(jrb_find_int(r->members, i));
                    fclose(u->fout);
                }
                 
                printf("room6..\n");
            }
            free_dllist(l);
            l = new_dllist();
        }
        
        printf("room7..\n");
        printf("room8\n");
    }
  
    printf("end room thread\n");
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