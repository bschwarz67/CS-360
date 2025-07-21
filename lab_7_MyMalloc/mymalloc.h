#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct flist {
   int size;
   struct flist *flink;
} *Flist;

void *my_malloc(size_t size);
void my_free(void *ptr);
void *free_list_begin();
void *free_list_next(void *node);
void coalesce_free_list();
char *malloc_begin = NULL;