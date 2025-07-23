#include "mymalloc.h"

void *my_malloc(size_t size) {
    void *ptr = NULL;
    unsigned int *ptr_u_int;
    int rem = -1, fm_size;
    Flist f, f2, prev;


    /*if there is no pre existing free list*/
    if(malloc_begin == NULL) {
        if(size > 8184) {
            rem = size % 8;
            if(rem > 0) size += (8 - rem + 8);
            else size += 8;
            ptr = (void *) sbrk(size);
            ptr_u_int = (unsigned int *) ptr;
            *ptr_u_int = size;
        }
        else {
            rem = size % 8;
            if(rem > 0) size += (8 - rem + 8);
            else size += 8;
            ptr = (void *) sbrk(8192);
            fm_size = 8192 - size;

            if(fm_size < 24) {
                ptr_u_int = (unsigned int *) ptr;
                *ptr_u_int = 8192;
            }
            else {
                malloc_begin = (char *) ptr;
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
                            malloc_begin = NULL;
                        }
                        else {
                            malloc_begin = (char *) f->flink;
                            f->flink = NULL;
                        }
                    }
                    else {
                        prev->flink = f->flink;
                        f->flink = NULL;
                    }
                    ptr_u_int = (unsigned int *) f;
                    *ptr_u_int = f->size;
                    ptr = (void *) f;
                }
                else {
                    ptr = (void *) f;
                    //take out part of the memory chunk and return, leave the rest

                    ptr = (void *) f; 
                    ptr += fm_size;
                    printf("ptr: 0x%lx \n",(long unsigned int) ptr);

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
            if(size > 8184) {
            rem = size % 8;
            if(rem > 0) size += (8 - rem + 8);
            else size += 8;
            ptr = (void *) sbrk(size);
            ptr_u_int = (unsigned int *) ptr;
            *ptr_u_int = size;
            }
            else {
                rem = size % 8;
                if(rem > 0) size += (8 - rem + 8);
                else size += 8;
                ptr = (void *) sbrk(8192);
                fm_size = 8192 - size;

                //here we need to set the next on malloc begin to the previous malloc begin and test *****
                malloc_begin = (char *) ptr;
                f = (Flist) ptr;
                f->size = fm_size;
                f->flink = NULL;

                ptr += fm_size;
                ptr_u_int = (unsigned int *) ptr;
                *ptr_u_int = size;
                
            }
        }
    }

    return ptr + 8;
}

void *free_list_begin() {
    Flist f = NULL;
    if(malloc_begin != NULL) f = (Flist) malloc_begin; 
    return f;
}
void *free_list_next(void *node) {
    Flist f = (Flist) node;
    return f->flink;
}

void my_free(void *ptr) {

}

int main() {
    Flist f;
    int *size;


  
    void* val = my_malloc((5000));

    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("pointer to the memory we asked for: 0x%lx, ", (long unsigned int) val);
    val = val - 8;
    size = (int *) val; 
    printf("size of the memory we asked for: %d\n", *size);
    
    val = my_malloc(800);

    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("pointer to the memory we asked for: 0x%lx, ", (long unsigned int) val);
    val = val - 8;
    size = (int *) val; 
    printf("size of the memory we asked for at 0x%lx: %d\n", (long unsigned int) val, *size);

    
    return 0;
}