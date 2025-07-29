#include "mymalloc.h"

char *malloc_head = NULL;

void *my_malloc(size_t size) {
    void *ptr = NULL;
    unsigned int *ptr_u_int;
    int rem = -1, fm_size, size_const = size;
    Flist f, f2, prev;


    /*if there is no pre existing free list*/
    if(malloc_head == NULL) {
        if(size > 8184) {
            rem = size % 8;
            if(rem > 0) size += (8 - rem + 8);
            else size += 8;
            ptr = (void *) sbrk(size);

            //printf("pointer to the memory from sbrk(): 0x%lx size: %d\n", (long unsigned int) ptr, size);

            ptr_u_int = (unsigned int *) ptr;
            *ptr_u_int = size;
        }
        else {
            rem = size % 8;
            if(rem > 0) size += (8 - rem + 8);
            else size += 8;
            ptr = (void *) sbrk(8192);
            //printf("pointer to the memory we asked for: 0x%lx size: %d\n", (long unsigned int) ptr, 8192);
            fm_size = 8192 - size;

            if(fm_size < 24) {
                ptr_u_int = (unsigned int *) ptr;
                *ptr_u_int = 8192;
            }
            else {
                malloc_head = (char *) ptr;
                f = (Flist) ptr;
                f->size = fm_size;
                f->flink = NULL;

                ptr += fm_size;
                ptr_u_int = (unsigned int *) ptr;
                *ptr_u_int = size;
            }
        }
    }
    /*if there is a preexisting free list, we loop through it and see if there is a free chunk big enough to provide what we asked for. If there is, we take off of it
    if not, we have to call sbrk() again*/
    else { 
        rem = size % 8;
        if(rem > 0) size += (8 - rem + 8);
        else size += 8;
        f = free_list_begin();
        prev = NULL;
        while(f != NULL) {
            if(f->size >= size) {
                fm_size = f->size - size;
                if(fm_size < 24) {
                    if(prev == NULL) {
                        if(f->flink == NULL) {
                            malloc_head = NULL;
                        }
                        else {
                            malloc_head = (char *) f->flink;
                        }
                    }
                    else {
                        prev->flink = f->flink;
                    }
                    ptr_u_int = (unsigned int *) f;
                    *ptr_u_int = f->size;
                    ptr = (void *) f;
                }
                else {
                    //take out part of the memory chunk and return, leave the rest

                    ptr = (void *) f; 
                    ptr += fm_size;
                    //printf("ptr: 0x%lx \n",(long unsigned int) ptr);

                    ptr_u_int = (unsigned int *) ptr;
                    *ptr_u_int = size;

                    ptr_u_int = (unsigned int *) f;
                    *ptr_u_int = fm_size;
                }
                break;
            }
            else {
                prev = f;
                f = free_list_next(f);
            }
        }
        if(ptr == NULL) { //we didnt find a suitable block in the list we can just call jbrk with the size and return it.
            size = size_const;
            if(size > 8184) {
                rem = size % 8;
                if(rem > 0) size += (8 - rem + 8);
                else size += 8;
                ptr = (void *) sbrk(size);
                //printf("pointer to the memory we asked for: 0x%lx size: %d\n", (long unsigned int) ptr, size);
                ptr_u_int = (unsigned int *) ptr;
                *ptr_u_int = size;
            }
            else {
                rem = size % 8;
                if(rem > 0) size += (8 - rem + 8);
                else size += 8;
                ptr = (void *) sbrk(8192);
                //printf("pointer to the memory we asked for: 0x%lx size: %d\n", (long unsigned int) ptr, 8192);
                fm_size = 8192 - size;

                //here we need to set the next on malloc begin to the previous malloc begin and test *****
                //now we need to test this after work.
                if(fm_size < 24) {
                    ptr_u_int = (unsigned int *) ptr;
                    *ptr_u_int = 8192;
                }
                else {
                    f2 = free_list_begin();
                    f = (Flist) ptr;
                    f->size = fm_size;
                    f->flink = f2;
                    malloc_head = (char *) ptr;
                    ptr += fm_size;
                    ptr_u_int = (unsigned int *) ptr;
                    *ptr_u_int = size;
                }
                
                
            }
        }
    }

    return ptr + 8;
}

void *free_list_begin() {
    Flist f = NULL;
    if(malloc_head != NULL) f = (Flist) malloc_head; 
    return f;
}
void *free_list_next(void *node) {
    Flist f = (Flist) node;
    return f->flink;
}

void my_free(void *ptr) {
    int *size;
    Flist f, f2;
    ptr = ptr - 8;
    size = (int *) ptr;
    f = (Flist) ptr; 
    f->size = *size;
    f2 = free_list_begin();
    f->flink = f2;
    malloc_head = (char *) f;

}

int cmpulong(const void *a, const void *b) {


    if(*((long unsigned int *) a) < *((long unsigned int *) b)) {
        return -1;
    }
    else if(*((long unsigned int *) a) > *((long unsigned int *) b)) {
        return 1;
    }
    return 0;
}

void coalesce_free_list() {
    Flist f, f2;
    int size = 0, i = 0, j;
    long unsigned int *ads;
    char *ptr, *ptr2;

    f = free_list_begin();
    while(f != NULL) {
        f = free_list_next(f);
        size++;
    }

    ads = malloc(sizeof(long unsigned int) * size);
    f = free_list_begin();
    while(f != NULL) {
        *(ads + i) = (long unsigned int) f;
        f = free_list_next(f);
        i++;
    }
    if(size > 0) {
        qsort(ads, size, sizeof(long unsigned int), cmpulong);
        i = 0;
        ptr = (char *) *(ads + i);
        malloc_head = ptr;
        f = (Flist) *(ads + i);

        for(i; i < size; i++) {
            if(i + 1 < size) {
                ptr2 = (char *) *(ads + i + 1);
                f2 = (Flist) ptr2;
                if(ptr + f->size == ptr2) {
                    f->size = f->size + f2->size;
                    printf("%d\n",f->size);
                }
                else {
                    f->flink = f2;
                    ptr = ptr2;
                    f = f2;
                    printf("%d\n",f->size);
                }
                
            }
        }
        f->flink = NULL;
    }
    
    
}


/*
int main() {
    Flist f;
    int *size;

    
    void *val = my_malloc((5000));
    void *val2 = my_malloc(800);
    my_free(val);
    my_free(val2);
    f = free_list_begin();

    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("\n");
    coalesce_free_list();
    f = free_list_begin();
    printf("\n");

    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    

    
    void* val = my_malloc((5000));
    
    
    
    void* val2 = my_malloc(800);
    

    void *val3 = my_malloc(5000);
    
    void *val4 = my_malloc(3184);

    void *val5 = my_malloc(4000);
    

    void *val6 = my_malloc(3176);

    void *val7 = my_malloc(3160);
    
    
    my_free(val);
    my_free(val2);
    my_free(val3);
    my_free(val4);
    //my_free(val5);
    my_free(val6);
    my_free(val7);


    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("\n");

    
    coalesce_free_list();
    printf("\n");
    f = free_list_begin();
    printf("%d\n",(int) f->size);
    printf("%d\n",(int) f->flink->size);
    printf("f: 0x%lx %d\n", (long unsigned int) f, (int) f->size);
    
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    
    return 0;
}

*/