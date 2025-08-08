#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/fields.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"

typedef struct {
  char *stdin;          /* Filename from which to redirect stdin.  NULL if empty.*/ 
  char *stdout;         /* Filename to which to redirect stdout.  NULL if empty.*/ 
  int append_stdout;    /* Boolean for appending.*/ 
  int wait;             /* Boolean for whether I should wait.*/ 
  int n_commands;       /* The number of commands that I have to execute*/ 
  int *argcs;           /* argcs[i] is argc for the i-th command*/ 
  char ***argvs;        /* argcv[i] is the argv array for the i-th command*/ 
  Dllist comlist;       /* I use this to incrementally read the commands.*/ 
} Command;


void free_command(Command *comm) {
    
    free(comm->stdin);
    free(comm->stdout);
    printf("number of commands: %d\n",comm->n_commands);
    for(int i = 0; i < comm->n_commands; i++) {
        printf("command # %d\n",i);
        for(int j = 0; j < comm->argcs[i] - 1; j++) {
            printf("%s\n", comm->argvs[i][j]);
            free(comm->argvs[i][j]);
        }
        printf("check\n");

        free(comm->argvs[i]);
        printf("check2\n");
    }
    free(comm->argvs);
    free(comm->argcs);
    free_dllist(comm->comlist);



    comm->stdin = NULL;
    comm->stdout = NULL;
    comm->append_stdout = 0;
    comm->wait = 1;
    comm->n_commands = 0;
    comm->comlist = new_dllist();
    printf("completed freeing everyting\n");

}

int main() {
    IS is;
    is = new_inputstruct(NULL);
    Command *comm = (Command *) malloc(sizeof(Command));
    comm->stdin = NULL;
    comm->stdout = NULL;
    comm->append_stdout = 0;
    comm->wait = 1;
    comm->n_commands = 0;
    comm->comlist = new_dllist();
    char **argv;
    int i, j;
    Dllist tmp;

    while (get_line(is) >= 0) {
        if(is->NF > 0 && (is->fields[0][0] != 35)) {
            printf("%d\n", is->NF);
            if(is->NF == 2 && strcmp(is->fields[0], "<") == 0) {
                comm->stdin = (char *) malloc((strlen(is->fields[1]) + 1) * sizeof(char));
                strcpy(comm->stdin, is->fields[1]);
            }
            else if(is->NF == 2 && strcmp(is->fields[0], ">") == 0) {
                comm->stdout = (char *) malloc((strlen(is->fields[1]) + 1) * sizeof(char));
                strcpy(comm->stdout, is->fields[1]);
                comm->append_stdout = 0;

            }
            else if(is->NF == 2 && strcmp(is->fields[0], ">>") == 0) {
                comm->stdout = (char *) malloc((strlen(is->fields[1]) + 1) * sizeof(char));
                strcpy(comm->stdout, is->fields[1]);
                comm->append_stdout = 1;

            }
            else if(is->NF == 1 && strcmp(is->fields[0], "NOWAIT") == 0) {
                comm->wait = 0;
                printf("nowait\n");

            }
            else if(!(is->NF == 1 && strcmp(is->fields[0], "END") == 0)) {
                printf("adding a line for command\n");
                comm->n_commands++;
                argv = (char **) malloc((is->NF + 1) * sizeof(char **));
                for(i = 0; i < is->NF; i++) {
                    argv[i] = (char *) malloc((strlen(is->fields[i]) + 1) * sizeof(char));
                    strcpy(argv[i], is->fields[i]);
                }
                argv[i] = NULL;
                dll_append(comm->comlist, new_jval_v(argv));

            }
            else { //once we get a END we execute, then we free all of the memory and return the command struct to a default state
                printf("end called\n");
                printf("number of commands %d\n", comm->n_commands);
                comm->argcs = (int *) malloc(comm->n_commands * sizeof(int));
                comm->argvs = (char ***) malloc(comm->n_commands * sizeof(char **));
                i = 0;
                dll_traverse(tmp, comm->comlist) {
                    argv = (char **) tmp->val.v;
                    j = 0;
                    while(argv[j] != NULL) {
                        j++;
                    }

                    comm->argcs[i] = j + 1;
                    printf("%d\n",j);
                    comm->argvs[i] = argv;

                    i++;

                }

                free_command(comm);

            }
        }

    
    }
}