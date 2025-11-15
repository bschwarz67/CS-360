#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "bonding.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"

struct global_info {
    pthread_mutex_t *lock;
    Dllist hs;
    Dllist os;
};

struct thread_info {
    int tid;
    int h1;
    int h2;
    int o;
    pthread_cond_t *ready;
};

void *initialize_v(char *verbosity)
{
  struct global_info *g;

  g = (struct global_info *) malloc(sizeof(struct global_info));
  g->lock = new_mutex();
  g->hs = new_dllist();
  g->os = new_dllist();

  return (void *) g;
}


void *hydrogen(void *arg)
{
    struct bonding_arg *a;
    struct global_info *g;
    struct thread_info *ti = malloc(sizeof(struct thread_info));
    struct thread_info *temp;
    struct thread_info *temp2;
    char *rv;
    int id;


    a = (struct bonding_arg *) arg;
    g = (struct global_info *) a->v;
    id = (int) a->id;

    pthread_mutex_lock(g->lock);
    if(!dll_empty(g->hs) && !dll_empty(g->os)) {
        temp = (struct thread_info *) g->hs->flink->val.v;
        temp2 = (struct thread_info *) g->os->flink->val.v;

        temp->h1 = id;
        temp->h2 = temp->tid;
        temp->o = temp2->tid;

        temp2->h1 = id;
        temp2->h2 = temp->tid;
        temp2->o = temp2->tid;

        ti->h1 = id;
        ti->h2 = temp->tid;
        ti->o = temp2->tid;

        dll_delete_node(g->hs->flink);
        dll_delete_node(g->os->flink);

        pthread_cond_signal(temp->ready);
        pthread_cond_signal(temp2->ready);
        pthread_mutex_unlock(g->lock);
        rv = Bond(ti->h1, ti->h2, ti->o);
        free(ti);
        return rv;


    }
    else {
        ti->tid = id;
        ti->ready = new_cond();
        dll_append(g->hs, new_jval_v((void *) ti));
        pthread_cond_wait(ti->ready, g->lock);
        pthread_mutex_unlock(g->lock);
        rv = Bond(ti->h1, ti->h2, ti->o);
        free(ti->ready);
        free(ti);
        return rv;
    }
    
  
}


void *oxygen(void *arg)
{
    struct bonding_arg *a;
    struct global_info *g;
    struct thread_info *ti = malloc(sizeof(struct thread_info));
    struct thread_info *temp;
    struct thread_info *temp2;
    char *rv;
    int id;


    a = (struct bonding_arg *) arg;
    g = (struct global_info *) a->v;
    id = (int) a->id;

    pthread_mutex_lock(g->lock);
    if(g->hs->flink != g->hs && g->hs->flink->flink != g->hs) {
        temp = (struct thread_info *) g->hs->flink->val.v;
        temp2 = (struct thread_info *) g->hs->flink->flink->val.v;

        temp->h1 = temp->tid;;
        temp->h2 = temp2->tid;
        temp->o = id;

        temp2->h1 = temp->tid;;
        temp2->h2 = temp2->tid;
        temp2->o = id;

        ti->h1 = temp->tid;;
        ti->h2 = temp2->tid;
        ti->o = id;

        dll_delete_node(g->hs->flink);
        dll_delete_node(g->hs->flink);

        pthread_cond_signal(temp->ready);
        pthread_cond_signal(temp2->ready);
        pthread_mutex_unlock(g->lock);
        rv = Bond(ti->h1, ti->h2, ti->o);
        free(ti);
        return (void *) rv;


    }
    else {
        ti->tid = id;
        ti->ready = new_cond();
        dll_append(g->os, new_jval_v((void *) ti));
        pthread_cond_wait(ti->ready, g->lock);
        pthread_mutex_unlock(g->lock);
        rv = Bond(ti->h1, ti->h2, ti->o);
        free(ti->ready);
        free(ti);
        return (void *) rv;
    }
    
}