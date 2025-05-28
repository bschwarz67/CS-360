#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int i, j;
    FILE *fd;
    char c = '1', *str_buff, *bit_buff;

    fd = fopen(argv[1], "r");
    while(1) {
        str_buff = (char *) malloc(sizeof(char) * 10001);
        bit_buff = (char *) malloc(sizeof(char) * 10001);
        j = 0;
        i = fread(&c, 1, 1, fd);

        if(i <= 0) break;
        if (c == 0) return 0; 
        str_buff[j] = c;
        j++;
        i = fread(&c, 1, 1, fd);

        while(c != 0 && i > 0) {
            str_buff[j] = c;
            i = fread(&c, 1, 1, fd);

            j++;
        }
        str_buff[j] = '\0'; 
        if(i <= 0) return 0;
        i = fread(&c, 1, 1, fd);

        if(c != 48 && c != 49) return 0;
        j = 0;
        bit_buff[j] = c;
        j++;
        i = fread(&c, 1, 1, fd);

        while(c != 0 && i > 0 && (c == 48 || c == 49)) {
            bit_buff[j] = c;
            i = fread(&c, 1, 1, fd);

            j++;
        }
        bit_buff[j] = '\0'; 
        if(i <= 0) break;
        printf("%s %s\n", str_buff, bit_buff);
    }

    
    fclose(fd);


    return 0;
}