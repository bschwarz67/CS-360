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

void tarc(const char *fn, JRB inodes)
{
  DIR *d;                   /* Return value of opendir(). */
  struct dirent *de;        /* Return value of each readdir() call. */
  struct stat buf;          /* The information about each file returned by stat() */
  int exists;               /* Return value of stat on each file. */
  long total_size;          /* The total size of all files. */

  int fn_size;              /* This is the length of fn -- so we can build the filename. */
  char *dir_fn;             /* This will be the filename including the directory. */
  int dir_fn_size;          /* This is the bytes in dir_fn is, in case we need to make it bigger. */
  int sz;

  Dllist directories, tmp;  /* Dllist of directory names, for doing recusion after closing. */

  /* Initialize */

  d = opendir(fn);
  if (d == NULL) {
    perror(fn);
    exit(1);
  }
  directories = new_dllist();

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

    /* Make sure this file isnt a symbolic link and that it isnt a . or .. directory */

    if (!S_ISLNK(buf.st_mode) && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
    

        /*add extra informtion if this inode hasnt been encountered before*/
        if (jrb_find_gen(inodes, new_jval_l(buf.st_ino), compare) == NULL) {

            jrb_insert_gen(inodes, new_jval_l(buf.st_ino), new_jval_i(0), compare);
            printf("%s\n", dir_fn);
        }


        /* Don't make the recursive call, but instead put the directory into the dllist. */

        if (S_ISDIR(buf.st_mode)) {
        dll_append(directories, new_jval_s(strdup(dir_fn)));
        }
    }
  }

  /* Make the recursive calls after you've closed the directory.  */

  closedir(d);

  dll_traverse(tmp, directories) {
    tarc(tmp->val.s, inodes);
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
    int i = 1, j = 0, k = 0;
    JRB inodes;

    inodes = make_jrb();
    if(argc != 2) {
        fprintf(stderr, "usage: bin/tarc {directory} > {[].tarc})\n");
        return 1;
    }
    for(j = strlen(argv[1]) - 1; j >= 0; j--) {
        if(argv[1][j] == '/') {
            i = 0;
            break;
        }
    }

    if(!i) {
        j++;
        i = strlen(argv[1]) - j;
        fn = malloc((i + 1) * sizeof(char));
        i = strlen(argv[1]) - 1;
        for(j; j <= i; j++) {
            fn[k] = argv[1][j];
            k++;
        }
        fn[k] = '\0';
        printf("%s\n", fn);
        i = 1000;
        sprintf(bytes, "%08lx", i);
        printf("%s\n",bytes);
        sprintf(cd, "cd %s/..", argv[1]);
        system(cd);
        tarc(fn, inodes);
    }
    else {
        tarc(argv[1], inodes);
    }

}
