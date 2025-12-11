#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"

#include <fcntl.h>
#include <unistd.h>


int compare(Jval v1, Jval v2)
{
  if (v1.l < v2.l) return -1;
  if (v1.l > v2.l) return 1;
  return 0;
}


int main(int argc, char **argv){
    char *buf = (char* ) malloc(sizeof(char) * 1000);
    char *path, *bytes;
    int len;
    unsigned long inode;
    unsigned short mode, fod;
    long mtime, size;
    JRB inodes = make_jrb();
    JRB tmp;
    JRB dirs = make_jrb(); //strings
    FILE *f;


    while(!feof(stdin)) {
        fread(&len, 1, 4, stdin);
        len++;
        printf("length of path: %d\n", len);

        path = (char *) malloc(sizeof(char) * len);
        fread(path, 1, len - 1, stdin);
        printf("path: %s\n",path);

        fread(&inode, 1, 8, stdin);
        printf("inode: %lu\n",inode);

        fread(&mode, 1, 4, stdin);
        printf("mode (octal): %o\n",mode);
        fod = mode / 512;
        fread(&mtime, 1, 8, stdin);
        printf("modtime: %ld\n",mtime);

        if(fod == 64) { // is a file
            tmp = jrb_find_gen(inodes, new_jval_l(inode), compare);
            if (tmp == NULL) {
                fread(&size, 1, 8, stdin);
                printf("file size: %ld\n",size);
                size++;
                bytes = (char *) malloc(sizeof(char) * size);
                fread(bytes, 1, size - 1, stdin);
                printf("the bytes in the file: %s\n",bytes);
                f = fopen(path, "w+"); //failing here when opening directories that dont have write permissions.
                printf("test\n");

                fwrite(bytes, size, 1, f);
                free(bytes);
                chmod(path, (mode_t) mode);
                fclose(f);
                
                jrb_insert_gen(inodes, new_jval_l(inode), new_jval_s(strdup(path)), compare);
            }
            else {
                link(path, tmp->val.s);
            }


        }
        else { //is a directory
            tmp = jrb_find_gen(inodes, new_jval_l(inode), compare);
            if (tmp == NULL) {
                mkdir(path, (mode_t) mode);
                jrb_insert_gen(inodes, new_jval_l(inode), new_jval_s(strdup(path)), compare);
            }
            else {
                link(path, tmp->val.s);
            }
        }
        free(path);
        
    }

}