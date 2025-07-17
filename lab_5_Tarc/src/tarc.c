#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"


int compare(Jval v1, Jval v2)
{
  if (v1.l < v2.l) return -1;
  if (v1.l > v2.l) return 1;
  return 0;
}

void tarc(const char *fn, JRB inodes, int flag, int index)
{
    DIR *d;                   /* Return value of opendir(). */
    struct dirent *de;        /* Return value of each readdir() call. */
    struct stat buf;          /* The information about each file returned by stat() */
    int exists;               /* Return value of stat on each file. */
    long total_size;          /* The total size of all files. */

    int fn_size;              /* This is the length of fn -- so we can build the filename. */
    char *dir_fn, c, *dir;
    int dir_fn_size;          /* This is the bytes in dir_fn is, in case we need to make it bigger. */
    int sz, dir_size;
    int len;
    FILE *fd;
    int i;

    Dllist directories, tmp;  /* Dllist of directory names, for doing recusion after closing. */


    /* Initialize */

    d = opendir(fn);
    if (d == NULL) {
        perror(fn);
        exit(1);
    }
    directories = new_dllist();


    /*output for the directory itself first time called*/
    
    if(flag == 0) {
        exists = lstat(fn, &buf);
        if (exists < 0) {
            fprintf(stderr, "%s not found\n", fn);
        } 
        else {
            if(index >= 0) {
                dir_size = strlen(fn) - index + 1;
                dir = (char *) malloc(dir_size * sizeof(char));
                for (i = index; i < strlen(fn); i++) {
                    dir[i - index] = fn[i];
                }
                dir[i-index] = '\0';
            }
            else {

                dir_size = strlen(fn) + 1;
                dir = (char *) malloc(dir_size * sizeof(char));
                for(i = 0; i < dir_size; i++) {
                    dir[i] = fn[i];
                }
                dir[i] = '\0';
            }
            len = strlen(dir);
            /*The size of the file's name, as a four-byte integer in little endian.*/
            fwrite(&len, 1, 4, stdout);
            /*The file's name. No null character. */
            fwrite(dir, 1, len, stdout);
            /*The file's inode, as an eight byte long in little endian*/
            fwrite(&buf.st_ino, 1, 8, stdout);
            /*The file's mode, as a four byte integer in little endian. */
            fwrite(&buf.st_mode, 1, 4, stdout);
            /*The file's last modification time, in seconds, as an eight byte long in little endian*/
            fwrite(&buf.st_mtime, 1, 8, stdout);
            free(dir);

        }
    }

  /* Start building the directory + files.   We'll start by setting dir_fn_size to fn_size+10,
     and we'll make it bigger as we need to.  It will be more efficient to use a number bigger
     than 10 for this, but 10 will let us debug the code if there's a problem. 

     I'm also setting up dir_fn to hold the directory name and a slash. */

    fn_size = strlen(fn);
    dir_fn_size = fn_size + 10;
    dir_fn = (char *) malloc(sizeof(char) * dir_fn_size);
    if (dir_fn == NULL) { perror("malloc dir_fn"); exit(1); }
    strcpy(dir_fn, fn);
    strcat(dir_fn + fn_size, "/");



  /* Run through the directory and run stat() on each file,  */

  for (de = readdir(d); de != NULL; de = readdir(d)) {

    /* First, we need to build dir_fn.  First check to see if it's big enough, and if not,
       we'll call realloc() to reallocate space.  Then we put the filename after the
       slash. */

    sz = strlen(de->d_name);
    if (dir_fn_size < fn_size + sz + 2) {    /* The +2 is for the slash and null character. */
      dir_fn_size = fn_size + sz + 10;
      dir_fn = realloc(dir_fn, dir_fn_size);
    }
    strcpy(dir_fn + fn_size + 1, de->d_name);
   
    exists = lstat(dir_fn, &buf);
    if (exists < 0) {
      fprintf(stderr, "Couldn't stat %s\n", dir_fn);
      exit(1);
    } 

    if(index >= 0) {
        dir_size = strlen(dir_fn) - index + 1;
        dir = (char *) malloc(dir_size * sizeof(char));
        for (i = index; i < strlen(dir_fn); i++) {
            dir[i - index] = dir_fn[i];
        }
        dir[i-index] = '\0';
    }
    else {
        dir_size = strlen(dir_fn) + 1;
        dir = (char *) malloc(dir_size * sizeof(char));
        for(i = 0; i < dir_size; i++) {
            dir[i] = dir_fn[i];
        }
        dir[i] = '\0';
    }

    /* Make sure this file isnt a symbolic link and that it isnt a . or .. directory */

    if (!S_ISLNK(buf.st_mode) && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
    
        /* Don't make the recursive call, dont output anything about the directory because it will be in next recursive call,
         but instead put the directory into the dllist. */

        if (S_ISDIR(buf.st_mode)) {
            dll_append(directories, new_jval_s(strdup(dir_fn)));
            /*The size of the file's name, as a four-byte integer in little endian.*/
            len = strlen(dir);
            fwrite(&len, 1, 4, stdout);
            /*The file's name. No null character. */
            fwrite(dir, 1, len, stdout);
            /*The file's inode, as an eight byte long in little endian*/
            fwrite(&buf.st_ino, 1, 8, stdout);
            /*The file's mode, as a four byte integer in little endian. */
            fwrite(&buf.st_mode, 1, 4, stdout);
            /*The file's last modification time, in seconds, as an eight byte long in little endian*/
            fwrite(&buf.st_mtime, 1, 8, stdout);
        }

        else {
            /*The size of the file's name, as a four-byte integer in little endian.*/
            len = strlen(dir);
            fwrite(&len, 1, 4, stdout);
            /*The file's name. No null character. */
            fwrite(dir, 1, len, stdout);
            /*The file's inode, as an eight byte long in little endian*/
            fwrite(&buf.st_ino, 1, 8, stdout);
            /*extra file information goes here*/
            if (jrb_find_gen(inodes, new_jval_l(buf.st_ino), compare) == NULL) {
                jrb_insert_gen(inodes, new_jval_l(buf.st_ino), new_jval_i(0), compare);
                /*The file's mode, as a four byte integer in little endian. */
                fwrite(&buf.st_mode, 1, 4, stdout);
                /*The file's last modification time, in seconds, as an eight byte long in little endian*/
                fwrite(&buf.st_mtime, 1, 8, stdout);
                /*The file's size, as an eight byte long in little endian.*/
                fwrite(&buf.st_size, 1, 8, stdout);
                /*The file's bytes. */
                fd = fopen(dir_fn, "r");      
                c = fgetc(fd);       
                while(c != EOF) {    
                    putchar(c);        
                    c = fgetc(fd);     
                }
                fclose(fd);
            }   
        }


    }
  }

  /* Make the recursive calls after you've closed the directory.  */

  closedir(d);

  dll_traverse(tmp, directories) {
    tarc(tmp->val.s, inodes, 1, index);
  }

  /* Clean up.  You need to free the strings inside the dllist, because you
     allocated them with strdup(), and they'll be a memory leak otherwise. */

  dll_traverse(tmp, directories) free(tmp->val.s);
  free_dllist(directories);
  free(dir_fn);

  return;
}

int main(int argc, char **argv){
    char *fn = NULL;
    char *bytes;
    char *cd;
    int i = 0, j = -1, k = 0;
    JRB inodes;

    inodes = make_jrb();
    if(argc != 2) {
        fprintf(stderr, "usage: bin/tarc {directory} > {[].tarc})\n");
        return 1;
    }

    for(i; i < strlen(argv[1]); i++) {
        if(argv[1][i] == '/') {
            j = i;
        }
    }
    tarc(argv[1], inodes, 0, j + 1);

}