#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>


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
    int len, r, s, flag = 1;
    unsigned long inode;
    unsigned short mode, fod;
    long mtime, size;
    JRB inodes = make_jrb();
    JRB tmp;
    Dllist files = new_dllist();
    Dllist tmp2;
    File *file; 
    FILE *f = NULL;
    struct utimbuf *time;
    r = fread(&len, 1, 4, stdin);
    if(!(r < 4)) {
        while(1) {
            if(flag == 1) {
                flag = 0;
            }
            else {
                r = fread(&len, 1, 4, stdin);
                if(r < 4) {
                    if (r == 0) {
                        break;
                    }
                    else {
                        fprintf(stderr, "error!\n");
                        return 0;
                    }
                }
            }
            len++;
            path = (char *) malloc(len);
            r = fread(path, 1, len - 1, stdin);
            if (r < len - 1) {
                fprintf(stderr, "error!\n");
                return 0;
            }
            path[len - 1] = '\0';

            r = fread(&inode, 1, 8, stdin);
            if (r < 8) {
                fprintf(stderr, "error!\n");
                return 0;
            }

            tmp = jrb_find_gen(inodes, new_jval_l(inode), compare);

            if (tmp == NULL) {

                r = fread(&mode, 1, 4, stdin);
                if (r < 4) {
                    fprintf(stderr, "error!\n");
                    return 0;
                }
                fod = mode / 512;
                r = fread(&mtime, 1, 8, stdin);
                if (r < 8) {
                    fprintf(stderr, "error!\n");
                    return 0;
                }

                if(fod == 64) { // is a file   
                    r = fread(&size, 1, 8, stdin);
                    if (r < 8) {
                        fprintf(stderr, "error!\n");
                        return 0;
                    };
                    bytes = (char *) malloc(sizeof(char) * size);
                    r = fread(bytes, 1, size, stdin);
                    if (r < size) {
                        fprintf(stderr, "error!\n");
                        return 0;
                    }
                    f = fopen(path, "w+");
                    if(f == NULL) {
                        fprintf(stderr, "error!\n");
                        return 0;
                    } 
                    s = chmod(path, 0777);
                    if(s == -1) {
                        fprintf(stderr, "error!\n");
                        return 0;
                    }
                    s = fwrite(bytes, 1, size, f);
                    if(s < size) {
                        fprintf(stderr, "error!\n");
                        return 0;
                    }
                    free(bytes);
                    fclose(f);

                }
                else { //is a directory
                    s = mkdir(path, 0777);
                    if(s == -1) {
                        fprintf(stderr, "error!\n");
                        return 0;
                    }
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
                s = link(tmp->val.s, path);
                if(s == -1) {
                    fprintf(stderr, "error!\n");
                    return 0;
                }
                s = chmod(path, 0777);
                if(s == -1) {
                    fprintf(stderr, "error!\n");
                    return 0;
                }
            }   
            free(path);   
        }
    }
    else {
        if (r != 0) {
            fprintf(stderr, "error!\n");
            return 0;
        }
    }
   
    dll_rtraverse(tmp2, files) {
        file = (File *) tmp2->val.v;
        time = (struct utimbuf *) malloc(sizeof(struct utimbuf));
        time->modtime = (time_t) file->mtime;
        time->actime = (time_t) file->mtime;
        utime(file->path, time);
        chmod(file->path, file->mode);
        free(time);
    }
    return 1;
}