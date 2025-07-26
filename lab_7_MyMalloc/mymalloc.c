#include "mymalloc.h"

void *my_malloc(size_t size) {
    void *ptr = NULL;
    unsigned int *ptr_u_int;
    int rem = -1, fm_size, size_const = size;
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
                //now we need to test this after work.
                f2 = free_list_begin();
                f = (Flist) ptr;
                f->size = fm_size;
                f->flink = f2;
                malloc_begin = (char *) ptr;

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
    int *size;
    Flist f, f2;
    ptr = ptr - 8;
    size = (int *) ptr;
    printf("%d \n", *size);
    printf("0x%lx\n",(long unsigned int) ptr);
    f = (Flist) ptr; 
    f->size = *size;
    f2 = free_list_begin();
    f->flink = f2;
    malloc_begin = (char *) f;

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


    val = my_malloc(5000);

    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("pointer to the memory we asked for: 0x%lx, ", (long unsigned int) val);
    val = val - 8;
    size = (int *) val; 
    printf("size of the memory we asked for at 0x%lx: %d\n", (long unsigned int) val, *size);




    val = my_malloc(3184);

    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("pointer to the memory we asked for: 0x%lx, ", (long unsigned int) val);
    val = val - 8;
    size = (int *) val; 
    printf("size of the memory we asked for at 0x%lx: %d\n", (long unsigned int) val, *size);


    val = my_malloc(4000);

    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("pointer to the memory we asked for: 0x%lx, ", (long unsigned int) val);
    val = val - 8;
    size = (int *) val; 
    printf("size of the memory we asked for at 0x%lx: %d\n", (long unsigned int) val, *size);


    val = my_malloc(3176);

    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("pointer to the memory we asked for: 0x%lx, ", (long unsigned int) val);
    val = val - 8;
    size = (int *) val; 
    printf("size of the memory we asked for at 0x%lx: %d\n", (long unsigned int) val, *size);



    //test edge case when the remainder of a free memory node is > 24
    val = my_malloc(3160);

    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }
    printf("pointer to the memory we asked for: 0x%lx, ", (long unsigned int) val);
    val = val - 8;
    size = (int *) val; 
    printf("size of the memory we asked for at 0x%lx: %d\n", (long unsigned int) val, *size);



    my_free(val + 8);
    f = free_list_begin();
    while(f != NULL) {
        printf("f: 0x%lx f size:%d\n", (long unsigned int) f, f->size);
        f = free_list_next(f);
    }




    return 0;
}