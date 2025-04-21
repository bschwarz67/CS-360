#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/fields.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"



typedef struct person {
    char *name;
    char *sex;
    struct person *mother;
    struct person *father;
    Dllist children;
  } Person;


int main() {
    IS is;
    JRB t, tmp;
    is = new_inputstruct(NULL);
    t = make_jrb();

    while (get_line(is) >= 0) {
        if (is->NF > 1) {
            printf("%d\n", is->line);
            printf("%s\n", is->fields[0]);
        }

    }
    



}