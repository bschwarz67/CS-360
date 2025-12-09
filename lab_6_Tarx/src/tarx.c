#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"





int main(int argc, char **argv){
    int size = 1000;
    char *buf = (char* ) malloc(sizeof(char) * 1000);
    char *dir;
    int len;
    unsigned long inode;
    unsigned short mode, fod;

    fread(&len, 1, 4, stdin);
    len++;
    printf("%d\n", len);

    dir = (char *) malloc(sizeof(char) * len);
    fread(dir, 1, len - 1, stdin);
    printf("%s\n",dir);

    fread(&inode, 1, 8, stdin);
    printf("%lu\n",inode);

    fread(&mode, 1, 4, stdin);
    printf("%o\n",mode);
    fod = mode / 512;
    printf("%o\n",fod);
    printf("%d\n",fod);
    /*
    while(!feof(stdin)) {



        fread(buf, size, 1, stdin);
        printf("%s\n",buf);
    }
        */

}