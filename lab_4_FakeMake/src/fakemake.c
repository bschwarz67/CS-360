#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/fields.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"

int main(int argc, char **argv)
{
    IS is;
    int i = 0, j = 0, k = 0, libs_len = 0, p = 0, c_size = 0, h_size = 0, l_size = 0, f_size = 0, o_size = 0;
    Dllist c, h, l, f, n, m, o_list;
    char *e, *o, *com, *libs, *flags;
    struct stat o_buf;
    struct stat c_buf;
    struct stat h_buf;
    struct stat l_buf;
    struct stat f_buf;
    c = new_dllist();
    h = new_dllist();
    l = new_dllist();
    f = new_dllist();
    o_list = new_dllist();
    
    
    if(argc == 1 || argc == 2) {

        if(argc == 1) is = new_inputstruct("fmakefile");
        else is = new_inputstruct(argv[1]);
        if (is == NULL) {
            fprintf(stderr, "Error: Unable to open file.\n");
            return 0;
        } 
        else {
            while (get_line(is) >= 0) { //read in the fakemakefile and process the lines
                if(is->NF > 0) { //none blank lines
                    if(strcmp(is->fields[0], "C") == 0) { //if the line starts with 'C' we put c file names into a list and keep track of the size a concatenated string would have
                        for (i = 1; i < is->NF; i++) {
                            dll_append(c, new_jval_s(strdup(is->fields[i])));
                            c_size += (strlen(is->fields[i])+1);
                        }
                    }
                    else if(strcmp(is->fields[0], "H") == 0) { 
                        for (i = 1; i < is->NF; i++) {
                            dll_append(h, new_jval_s(strdup(is->fields[i])));
                            h_size += (strlen(is->fields[i])+1);
                        }
                    }
                    else if(strcmp(is->fields[0], "L") == 0) { 
                        for (i = 1; i < is->NF; i++) {
                            dll_append(l, new_jval_s(strdup(is->fields[i])));
                            l_size += (strlen(is->fields[i])+1);
                            libs_len += (is->NF - 1);
                        }
                    }
                    else if(strcmp(is->fields[0], "F") == 0) { 
                        for (i = 1; i < is->NF; i++) {
                            dll_append(f, new_jval_s(strdup(is->fields[i])));
                            f_size += (strlen(is->fields[i])+1);
                        }
                    }
                    else if(strcmp(is->fields[0], "E") == 0) { //save the name of the executable
                        e = (char*) malloc((strlen(is->fields[1]) + 1) * sizeof(char));
                        strcpy(e, is->fields[1]);
                    }
                    else {
                        fprintf(stderr, "Error: Invalid line descriptor.\n");
                        return 0;
                    }
                } 
            }

            
            //now try to process the ages of the files to know if we need to recompile/ check the files exist


            //do flags
            flags = malloc(f_size * sizeof(char));
            i = 0;
            dll_traverse(n, f) {
                strcpy(flags + i, n->val.s);
                j = strlen(n->val.s);
                strcpy(flags + i + j, " ");
                i += (j + 1);
            }


            //do .c files

            dll_traverse(n, c) {
                if (stat(n->val.s, &c_buf) != 0) {
                    fprintf(stderr, "No file found: %s\n", n->val.s);
                    return 0;
                }
                else {
                    i = strlen(n->val.s);
                    o = (char *) malloc((i + 1) * sizeof(char));
                    strcpy(o, n->val.s);
                    o[i - 1] = 'o';
                    if(stat(o, &o_buf) != 0) { //if there are no .o files make the call string and then call it.
                        com = malloc((7 + strlen(flags) + strlen(n->val.s)));
                        strcpy(com, "gcc -c ");
                        strcpy(com + 7, flags);
                        strcpy(com + 7 + strlen(flags), n->val.s);
                        printf("%s\n", com);
                        if(system(com) != 0) {
                            fprintf(stderr, "Problem compiling: %s\n", com);
                            return 0;
                        }
                        else {
                            k = 1;

                        }

                    }
                    else { //check if .c is more recent than .o, or if any of the .h files are more recent, recompile
                        if(c_buf.st_mtime > o_buf.st_mtime) {
                            com = malloc((7 + strlen(flags) + strlen(n->val.s)));
                            strcpy(com, "gcc -c ");
                            strcpy(com + 7, flags);
                            strcpy(com + 7 + strlen(flags), n->val.s);
                            printf("%s\n", com);
                            if(system(com) != 0) {
                                fprintf(stderr, "Problem compiling: %s\n", com);
                                return 0;
                            }
                            else {
                                k = 1;
                            }
                        }
                        else {
                            dll_traverse(m, h) {
                                if (stat(m->val.s, &h_buf) != 0) {
                                    fprintf(stderr, "No file found: %s\n", m->val.s);
                                    return 0;
                                }
                                else {
                                    //printf("%d %d\n",h_buf.st_mtime,o_buf.st_mtime);
                                    if(h_buf.st_mtime > o_buf.st_mtime) {
                                        com = malloc((7 + strlen(flags) + strlen(n->val.s)));
                                        strcpy(com, "gcc -c ");
                                        strcpy(com + 7, flags);
                                        strcpy(com + 7 + strlen(flags), n->val.s);
                                        printf("%s\n", com);
                                        if(system(com) != 0) {
                                            fprintf(stderr, "Problem compiling: %s\n", com);
                                            return 0;
                                        }
                                        else {
                                            k = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    dll_append(o_list, new_jval_s(strdup(o)));
                    o_size += (strlen(o) + 1);
                } 
            }


            //do libraries
            libs = malloc(l_size * sizeof(char));
            i = 0;
            dll_traverse(n, l) {
                
                p++;
                strcpy(libs + i, n->val.s);
                j = strlen(n->val.s);
                if(p == libs_len) {
                    i += j;
                }
                else {
                    strcpy(libs + i + j, " ");
                    i += (j + 1); 
                }
            }


            //do .o files compiled to executable
            if(k == 1) {
                i = 0;
                com = malloc((7 + strlen(e) + 1 + f_size + 1 + o_size + 1 + l_size + 1) * sizeof(char));
                strcpy(com + i, "gcc -o ");
                i += 7;
                strcpy(com + i, e);
                i += strlen(e);
                strcpy(com + i, " ");
                i += 1;
                strcpy(com + i, flags);
                i += f_size;


                dll_traverse(n, o_list) {
                    strcpy(com + i, n->val.s);
                    j = strlen(n->val.s);
                    strcpy(com + i + j, " ");
                    i += (j + 1);
                }
                strcpy(com + i, libs);
                printf("%s\n", com);
                if(system(com) != 0) {
                    fprintf(stderr, "Problem compiling: %s\n", com);
                    return 0;
                }
            }
            else {
                printf("Nothing to do.\n");
            }
        }
    }
    else {
        fprintf(stderr, "Usage: fakemake [ description-file ].\n");
        return 0;
    }
    


  return 0;
}