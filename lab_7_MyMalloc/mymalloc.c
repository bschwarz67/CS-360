#include "mymalloc.h"

void *my_malloc(size_t size) {
    void *ptr;

    unsigned int *ptr_u_int;
    int rem = -1, fm_size;


    

    if(size > 8184) {
        if(malloc_begin == NULL) {
            rem = size % 8;
            if(rem > 0) size += (8 - rem + 8);
            else size += 8;
            ptr = (void *) sbrk(size);
            ptr_u_int = (unsigned int *) ptr;
            *ptr_u_int = size;
        }

    }
    else {
        if(malloc_begin == NULL) {
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
                ptr += fm_size;
                ptr_u_int = (unsigned int *) ptr;
                *ptr_u_int = size;
            }
        }
        
    }
    return ptr + 8;
}

int main() {

    void* val = my_malloc((5000));
    printf("0x%lx\n", (long unsigned int) val);
    printf("0x%lx\n", (long unsigned int) malloc_begin);
    val = val - 8;
    printf("0x%lx\n", (long unsigned int) val);
    int *size = (int *) val; 
    printf("%d\n", *size);
    return 0;
}