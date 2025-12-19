#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef struct huff_node {
    struct huff_node *zero;
    struct huff_node *one;
    char *s_zero;
    char *s_one;
  } HN;
  

int main(int argc, char **argv)
{
    int i, j, k, l, m, n;
    FILE *fd;
    char c = '1', *str_buff, *bit_buff;
    HN *hn, *hn_root;
    hn = malloc(sizeof(HN));
    hn_root = hn;
    hn->zero = NULL;
    hn->one = NULL;
    fd = fopen(argv[1], "r");
    while(1) {
        str_buff = (char *) malloc(sizeof(char) * 10001);
        bit_buff = (char *) malloc(sizeof(char) * 10001);
        j = 0;
        i = fread(&c, 1, 1, fd);

        if(i <= 0) {
            free(str_buff);
            free(bit_buff);
            break;
        }
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
        hn = hn_root;
        for(k = 0; k < j; k++) {
            if(k == j - 1) {
                if(bit_buff[k] == '1') {
                    hn->s_one = malloc(sizeof(char) * 10001); // new
                    strcpy(hn->s_one, str_buff);
                }
                else {
                    hn->s_zero = malloc(sizeof(char) * 10001); //new
                    strcpy(hn->s_zero, str_buff);
                } 
            }
            else if(bit_buff[k] == '1') {
                if(hn->one == NULL) {
                    hn->one = malloc(sizeof(HN));
                    hn->one->s_zero = NULL;
                    hn->one->s_one = NULL;
                    hn = hn->one;
                }
                else hn = hn->one;
            }
            else {
                if(hn->zero == NULL) {
                    hn->zero = malloc(sizeof(HN));
                    hn->zero->s_zero = NULL;
                    hn->zero->s_one = NULL;
                    hn = hn->zero;
                }
                else hn = hn->zero;
            }
        }
        hn = hn_root;
        if(i <= 0) {
            free(str_buff);
            free(bit_buff);
            break;
        }

    }
    fclose(fd);
    fd = fopen(argv[2], "r");


    j = 0;
    k = 0;
    fseek(fd, -4, SEEK_END);
    if(fread(&i, 1, 1, fd) <= 0) {
        fprintf(stderr, "Error: file is not the correct size.\n");
        return 0;
    }
    for(j = 0; j < 8; j++) {
        k += pow(2, j) * (i % 2);
        i = i / 2;
    }
    if(fread(&i, 1, 1, fd) <= 0) {
        fprintf(stderr, "Error: file is not the correct size.\n");
        return 0;
    }
    for(j = 8; j < 16; j++) {
        k += pow(2, j) * (i % 2);
        i = i / 2;
    }
    if(fread(&i, 1, 1, fd) <= 0) {
        fprintf(stderr, "Error: file is not the correct size.\n");
        return 0;
    }
    for(j = 16; j < 24; j++) {
        k += pow(2, j) * (i % 2);
        i = i / 2;
    }
    if(fread(&i, 1, 1, fd) <= 0) {
        fprintf(stderr, "Error: file is not the correct size.\n");
        return 0;
    }
    for(j = 24; j < 32; j++) {
        k += pow(2, j) * (i % 2);
        i = i / 2;
    }
    fseek(fd, 0, SEEK_SET);
    
    m = 0;
    n = k;
    if(n % 8 != 0) j = (n / 8) + 1;
    else j = n / 8;


    i = 1;
    k = 0;
    while(i > 0) {
        i = fread(&c, 1, 1, fd);
        if(i > 0) k++; 
    }
    if(k - 4 != j) {
        fprintf(stderr, "Error: Total bits = %d, but file's size is %d\n", n, k);
        return 0;
    }
    bit_buff = (char *) malloc(j * 8);

    
    fseek(fd, 0, SEEK_SET);
    for(k = 0; k < j; k++){
        fread(&i, 1, 1, fd);


        for(l = 0; l < 8; l++) {
            if(i % 2 == 0) bit_buff[m] = '0';
            else bit_buff[m] = '1';
            i = i / 2;
            m++;
        }
        
    }
    bit_buff[m] = '\0';
    i = 0;
    hn = hn_root;
    while(i < n) { 
        if(hn->zero == NULL && bit_buff[i] == '0') {
            if(hn->s_zero == NULL) {
                fprintf(stderr, "Unrecognized bits\n");
                return 0;
            }
            printf("%s", hn->s_zero);
            hn = hn_root;
        }
        else if(hn->one == NULL && bit_buff[i] == '1') {
            if(hn->s_one == NULL) {
                fprintf(stderr, "Unrecognized bits\n");
                return 0;
            }
            printf("%s", hn->s_one);
            hn = hn_root;
        }
        else if(hn->zero != NULL && bit_buff[i] == '0') {
            hn = hn->zero;
        }
        else if(hn->one != NULL && bit_buff[i] == '1') {
            hn = hn->one;
        }
        else {
            return 0;
        }
        i++;   
    }
    
    
    
    return 0;
}