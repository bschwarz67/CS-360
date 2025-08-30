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
    pthread_t tid; //pthread_self()
    pthread_t h1;
    pthread_t h2;
    pthread_t o;
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
    
  
}


void *oxygen(void *arg)
{
    
}