#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/fields.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"



typedef struct person {
    int visited;
    char *name;
    char *sex;
    struct person *mother;
    struct person *father;
    Dllist children;
  } Person;


char *concat_name(IS is) {
    int nsize, i;
    char *name;
    nsize = strlen(is->fields[1]);
    for (i = 2; i < is->NF; i++) nsize += (strlen(is->fields[i])+1);
    name = malloc( sizeof(char) * (nsize+1));
    strcpy(name, is->fields[1]);
    nsize = strlen(is->fields[1]);
    for (i = 2; i < is->NF; i++) {
        name[nsize] = ' ';
        strcpy(name+nsize+1, is->fields[i]);
        nsize += strlen(name+nsize);
    }
    return name;
}

int check_cycles(JRB t) {
    JRB tmp;
    Person *p;
    int res = 0;
    jrb_traverse(tmp, t) {
        p = (Person *) tmp->val.v;
        if(p->visited == 0) {
            res = DFS(p);
            if(res == 1) return 1;
        }
    }
    return 0;
}
int DFS(Person *p) {
    Dllist d;
    Person *c;
    p->visited = 1;
    dll_traverse(d, p->children) {
        c = (Person *) d->val.v;
        if(c->visited == 1) {
            p->visited = 0;
            return 1;
        }
        if(DFS(c) == 1) {
            p->visited = 0;
            return 1;
        }
        
    }
    p->visited = 0;
    return 0;
}

void print(JRB t) {
    JRB tmp;
    Person *p;
    jrb_traverse(tmp, t) {
        p = (Person *) tmp->val.v;
        if(p->visited == 0) TS(p);
    }
    return;
}

void TS(Person *p) {
    if(p->father != NULL && p->father->visited == 0) {
        TS(p->father);
    }
    if(p->mother != NULL && p->mother->visited == 0) {
        TS(p->mother);
    }
    p->visited = 1;
    printf("%s\n", p->name);

    if(strcmp(p->sex, "M") == 0) printf("Sex: %s\n", "Male");
    else if(strcmp(p->sex, "F") == 0) printf("Sex: %s\n", "Female");
    else printf("Sex: %s\n", "Unknown");

    if(p->father == NULL) printf("Father: Unknown\n");
    else printf("Father: %s\n", p->father->name);
    if(p->mother == NULL) printf("Mother: Unknown\n");
    else printf("Mother: %s\n", p->mother->name);

    printf("Children: ");
    if(dll_empty(p->children) == 1) printf("None\n");
    else {
        Dllist d;
        Person *c;
        dll_traverse(d, p->children) {
            c = (Person *) d->val.v;
            printf("\n%s",c->name);            
        }
    }
    printf("\n\n");
    return;
}

int main() {
    IS is;
    JRB t, tmp;
    char *name;
    int flag = 0, line = 0;
    Dllist d;
    Person *p, *r, *c, *y;
    is = new_inputstruct(NULL);
    t = make_jrb();

    while (get_line(is) >= 0) {
        if (is->NF > 0) {
            if(strcmp(is->fields[0], "PERSON") == 0) {
                line += 1;
                name = concat_name(is);
                tmp = jrb_find_str(t, name);
                if(tmp == NULL) {
                    p = malloc(sizeof(Person));
                    p->visited = 0;
                    p->name = malloc(sizeof(char) * (strlen(name) + 1));
                    strcpy(p->name, name);
                    
                    p->sex = malloc(sizeof(char) * 2);
                    strcpy(p->sex, "U");
                    p->mother = NULL;
                    p->father = NULL;
                    p->children = new_dllist();
                    jrb_insert_str(t, name, new_jval_v((void *) p));
                    tmp = jrb_find_str(t, name);
                    y = (Person *) tmp->val.v;

                }
                else {
                    p = (Person *) tmp->val.v;
                }

            }
            else if(strcmp(is->fields[0], "SEX") == 0) {
                line += 1;
                name = concat_name(is);
                
                if(strcmp(p->sex, "M") == 0 && strcmp(name, "F") == 0) {
                    fprintf(stderr, "Bad input - sex mismatch on line %d\n", line);
                    return 0;
                }
                else if(strcmp(p->sex, "F") == 0 && strcmp(name, "M") == 0) {
                    fprintf(stderr, "Bad input - sex mismatch on line %d\n", line);
                    return 0;
                }
                
                else {

                    p->sex = malloc(sizeof(char) * 2);
                    strcpy(p->sex, name);
                }
                    
            }
            else if(strcmp(is->fields[0], "FATHER") == 0) {
                line += 1;
                name = concat_name(is);
                tmp = jrb_find_str(t, name);
                if(tmp == NULL) {
                    r = malloc(sizeof(Person));
                    r->visited = 0;
                    r->name = malloc(sizeof(char) * (strlen(name) + 1));
                    strcpy(r->name, name);
                    r->sex = malloc(sizeof(char) * 2);
                    strcpy(r->sex, "M");
                    r->mother = NULL;
                    r->father = NULL;
                    r->children = new_dllist();
                    dll_append(r->children, new_jval_v((void *) p));
                    if(p->father == NULL) p->father = r;
                    else{
                        fprintf(stderr, "Bad input -- child with two fathers on line %d\n", line);
                        return 0;
                    } 
                    jrb_insert_str(t, name, new_jval_v((void *) r));

                }
                else {
                    r = (Person *) tmp->val.v;
                    if(strcmp(r->sex, "F") == 0) {
                        fprintf(stderr, "Bad input - sex mismatch on line %d\n", line);
                        return 0;
                    } 
                    else {
                        r->sex = malloc(sizeof(char) * 2);

                        strcpy(r->sex, "M");
                    }
                    dll_traverse(d, r->children) {
                        c = (Person *) d->val.v;
                        if(strcmp(c->name, p->name) == 0) flag = 1;
                    }
                    if(!flag) dll_append(r->children, new_jval_v((void *) p));
                    flag = 0;
                    if(p->father == NULL || strcmp(p->father->name, r->name) == 0) p->father = r;
                    else{
                        fprintf(stderr, "Bad input -- child with two fathers on line %d\n", line);
                        return 0;
                    }
                }
                tmp = jrb_find_str(t, name);
                y = (Person *) tmp->val.v;
            }
            else if(strcmp(is->fields[0], "MOTHER") == 0) {
                line += 1;
                name = concat_name(is);
                tmp = jrb_find_str(t, name);
                if(tmp == NULL) {
                    r = malloc(sizeof(Person));
                    r->visited = 0;
                    r->name = malloc(sizeof(char) * (strlen(name) + 1));
                    strcpy(r->name, name);
                    r->sex = malloc(sizeof(char) * 2);
                    strcpy(r->sex, "F");
                    r->mother = NULL;
                    r->father = NULL;
                    r->children = new_dllist();
                    dll_append(r->children, new_jval_v((void *) p));
                    if(p->mother == NULL) p->mother = r;
                    else{
                        fprintf(stderr, "Bad input -- child with two mothers on line %d\n", line);
                        return 0;
                    } 
                    


                    jrb_insert_str(t, name, new_jval_v((void *) r));

                }
                else {
                    r = (Person *) tmp->val.v;
                    if(strcmp(r->sex, "M") == 0) {
                        fprintf(stderr, "Bad input - sex mismatch on line %d\n", line);
                        return 0;
                    } 
                    else {
                        r->sex = malloc(sizeof(char) * 2);
                        strcpy(r->sex, "F");
                    }
                    dll_traverse(d, r->children) {
                        c = (Person *) d->val.v;
                        if(strcmp(c->name, p->name) == 0) flag = 1;
                    }
                    if(!flag) dll_append(r->children, new_jval_v((void *) p));
                    flag = 0;
                    if(p->mother == NULL || strcmp(p->mother->name, r->name) == 0) p->mother = r;
                    else{
                        fprintf(stderr, "Bad input -- child with two mothers on line %d\n", line);
                        return 0;
                    }
                }
                tmp = jrb_find_str(t, name);
                y = (Person *) tmp->val.v;
            }
            else if(strcmp(is->fields[0], "FATHER_OF") == 0) {
                line += 1;
                name = concat_name(is);
                tmp = jrb_find_str(t, name);
                if(strcmp(p->sex, "F") == 0) { 
                    fprintf(stderr, "Bad input - sex mismatch on line %d\n", line);
                    return 0;
                }
                else {
                    p->sex = malloc(sizeof(char) * 2);
                    strcpy(p->sex, "M");
                }
                if(tmp == NULL) {
                    r = malloc(sizeof(Person));
                    r->visited = 0;
                    r->name = malloc(sizeof(char) * (strlen(name) + 1));
                    strcpy(r->name, name);
                    r->sex = malloc(sizeof(char) * 2);
                    strcpy(r->sex, "U");
                    r->mother = NULL;
                    r->father = p;
                    r->children = new_dllist();
                    dll_append(p->children, new_jval_v((void *) r));
                    jrb_insert_str(t, name, new_jval_v((void *) r));

                }
                else {
                    r = (Person *) tmp->val.v;
                    dll_traverse(d, p->children) {
                        c = (Person *) d->val.v;
                        if(strcmp(c->name, r->name) == 0) flag = 1;
                    }
                    if(!flag) dll_append(p->children, new_jval_v((void *) r));
                    flag = 0;
                    if(r->father == NULL || strcmp(r->father->name, p->name) == 0) r->father = p;
                    else{
                        fprintf(stderr, "Bad input -- child with two fathers on line %d\n", line);
                        return 0;
                    }
                }
                tmp = jrb_find_str(t, name);
                y = (Person *) tmp->val.v;

            }
            else { // else if(is->fields[0] == "MOTHER OF") 
                line += 1;
                name = concat_name(is);
                tmp = jrb_find_str(t, name);
                if(strcmp(p->sex, "M") == 0) { 
                    fprintf(stderr, "Bad input - sex mismatch on line %d\n", line);
                    
                    return 0;
                }
                else {
                    p->sex = malloc(sizeof(char) * 2);
                    strcpy(p->sex, "F");
                }
                if(tmp == NULL) {
                    r = malloc(sizeof(Person));
                    r->visited = 0;
                    r->name = malloc(sizeof(char) * (strlen(name) + 1));
                    strcpy(r->name, name);
                    r->sex = malloc(sizeof(char) * 2);
                    strcpy(r->sex, "U");
                    r->mother = p;
                    r->father = NULL;
                    r->children = new_dllist();
                    dll_append(p->children, new_jval_v((void *) r));
                    jrb_insert_str(t, name, new_jval_v((void *) r));

                }
                else {
                    r = (Person *) tmp->val.v;
                    dll_traverse(d, p->children) {
                        c = (Person *) d->val.v;
                        if(strcmp(c->name, r->name) == 0) flag = 1;
                    }
                    if(!flag) dll_append(p->children, new_jval_v((void *) r));
                    flag = 0;
                    if(r->mother == NULL || strcmp(r->mother->name, p->name) == 0) r->mother = p;
                    else{
                        fprintf(stderr, "Bad input -- child with two mothers on line %d\n", line);
                        return 0;
                    }
                }
                tmp = jrb_find_str(t, name);
                y = (Person *) tmp->val.v;
                
            }
        }
        else {
            line += 1;
        }

    }
    if(check_cycles(t)) {
        fprintf(stderr, "Bad input -- cycle in specification\n");
        return 1;
    }
    print(t);


    return 1;
    
}
