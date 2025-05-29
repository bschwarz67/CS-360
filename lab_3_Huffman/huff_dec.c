#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct huff_node {
    struct huff_node *zero;
    struct huff_node *one;
    char *s_zero;
    char *s_one;
  } HN;
  

int main(int argc, char **argv)
{
    int i, j, k;
    FILE *fd;
    char c = '1', *str_buff, *bit_buff;
    HN *hn, *hn_root;
    hn = malloc(sizeof(HN));
    hn_root = hn;
    hn->zero = NULL;
    hn->one = NULL;
    hn->s_zero = malloc(sizeof(char) * 10001);
    hn->s_one = malloc(sizeof(char) * 10001);


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
        for(k = 0; k < j - 1; k++) {
            if(k == j - 2) {
                if(bit_buff[k] == '1') strcpy(hn->s_one, str_buff);
                else strcpy(hn->s_zero, str_buff);
            }
            else if(bit_buff[k] == '1') {
                if(hn->one == NULL) {
                    hn->one = malloc(sizeof(HN));
                    hn->one->zero = NULL;
                    hn->one->one = NULL;
                    hn->one->s_zero = malloc(sizeof(char) * 10001);
                    hn->one->s_one = malloc(sizeof(char) * 10001);
                    hn = hn->one;
                }
                else hn = hn->one;
            }
            else {
                if(hn->zero == NULL) {
                    hn->zero = malloc(sizeof(HN));
                    hn->zero->zero = NULL;
                    hn->zero->one = NULL;
                    hn->zero->s_zero = malloc(sizeof(char) * 10001);
                    hn->zero->s_one = malloc(sizeof(char) * 10001);
                    hn = hn->zero;
                }
                else hn = hn->zero;
            }
        }
        hn = hn_root;

    }

    
    fclose(fd);


    return 0;
}