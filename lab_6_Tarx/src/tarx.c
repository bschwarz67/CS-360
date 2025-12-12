#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"
#include <fcntl.h>
#include <unistd.h>


typedef struct {
    char *path;
    unsigned short mode;
    long mtime;
} File;


int compare(Jval v1, Jval v2)
{
  if (v1.l < v2.l) return -1;
  if (v1.l > v2.l) return 1;
  return 0;
}


int main(int argc, char **argv){
    char *buf = (char* ) malloc(sizeof(char) * 1000);
    char *path, *bytes;
    int len, r, flag = 1;
    unsigned long inode;
    unsigned short mode, fod;
    long mtime, size;
    JRB inodes = make_jrb();
    JRB tmp;
    Dllist files = new_dllist();
    Dllist tmp2;
    File *file; 
    FILE *f;
    //TODO: make a list of directories and files in the order that they are read in. Create the directories/files and make them writable. then go through the list in reverse and then change the modification time,
    //then change the mode to the corect mode.

    r = fread(&len, 1, 4, stdin);
    while(r > 0) {
        if(flag == 1) {
            flag = 0;
        }
        else {
            r = fread(&len, 1, 4, stdin);
            if (r <= 0) break;
        }
        len++;
        printf("length of path: %d\n", len);

        path = (char *) malloc(sizeof(char) * len);
        r = fread(path, 1, len - 1, stdin);
        if (r <= 0) break;
        path[len - 1] = '\0';
        printf("path: %s\n",path);

        r = fread(&inode, 1, 8, stdin);
        if (r <= 0) break;
        printf("inode: %lu\n",inode);

        tmp = jrb_find_gen(inodes, new_jval_l(inode), compare);

        if (tmp == NULL) {

            r = fread(&mode, 1, 4, stdin);
            if (r <= 0) break;
            printf("mode (octal): %o\n",mode);
            fod = mode / 512;
            r = fread(&mtime, 1, 8, stdin);
            if (r <= 0) break;
            printf("modtime: %d\n",mtime);

            if(fod == 64) { // is a file
                
                
                r = fread(&size, 1, 8, stdin);
                if (r <= 0) break;
                printf("file size: %ld\n",size);
                //size++;
                bytes = (char *) malloc(sizeof(char) * size);
                r = fread(bytes, sizeof(char), size /*- 1*/, stdin);
                if (r <= 0) break;
                //bytes[size - 1] = '\0';
                printf("the bytes in the file: %s\n",bytes);
                f = fopen(path, "w+"); 
                chmod(path, 0777);
                printf("here\n");
                fwrite(bytes, sizeof(char), size /*- 1*/, f);
                printf("heere\n");
                free(bytes);
                //chmod(path, (mode_t) mode);
                fclose(f);

            }
            else { //is a directory
                printf("inserted\n");
                mkdir(path, 0777);
                
            }
            jrb_insert_gen(inodes, new_jval_l(inode), new_jval_s(strdup(path)), compare);
            file = (File*) malloc(sizeof(File));
            file->path = (char *) malloc(sizeof(char) * len);
            strcpy(file->path, path);
            file->mode = mode;
            file->mtime = mtime;
            dll_append(files, new_jval_v(file));
        }
        else {
            //f = fopen(path, "w+");
            link(tmp->val.s, path);
            chmod(path, 0777);
            //fclose(f);
        }   
        free(path);
        
    }
    printf("continue\n");
    dll_rtraverse(tmp2, files) {
        file = (File *) tmp2->val.v;
        chmod(file->path, file->mode);
    }
}