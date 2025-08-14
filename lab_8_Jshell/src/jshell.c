#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
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
    for(int i = 0; i < comm->n_commands; i++) {
        for(int j = 0; j < comm->argcs[i] - 1; j++) {
            free(comm->argvs[i][j]);
        }

        free(comm->argvs[i]);
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
    //printf("completed freeing everything\n");

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
    int i, j, dummy1, dummy2, prev = -1, fd;
    Dllist tmp;
    int pipefd[2];
    char *s;
    JRB t, t_tmp;

    while (get_line(is) >= 0) {

        if(is->NF > 0 && (is->fields[0][0] != 35)) {
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

            }
            else if(!(is->NF == 1 && strcmp(is->fields[0], "END") == 0)) {
                comm->n_commands++;
                argv = (char **) malloc((is->NF + 1) * sizeof(char *));
                for(i = 0; i < is->NF; i++) {
                    argv[i] = (char *) malloc((strlen(is->fields[i]) + 1) * sizeof(char));
                    strcpy(argv[i], is->fields[i]);
                }
                argv[i] = NULL;
                dll_append(comm->comlist, new_jval_v(argv));

            }
            else { //once we get a END we execute, then we free all of the memory and return the command struct to a default state
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
                    comm->argvs[i] = argv;

                    i++;

                }
                t = make_jrb();
                for(i = 0; i < comm->n_commands; i++) {
                    fflush(stdin);
                    fflush(stdout);
                    fflush(stderr);
                    j = pipe(pipefd);
                    if (j < 0) {
                        perror("pipe");
                        exit(1);
                    }
                    j = fork();
                    if(j == 0) {

                        if(i == comm->n_commands - 1) {
                            if(comm->stdout != NULL) {
                                if(comm->append_stdout == 0) fd = open(comm->stdout, O_WRONLY | O_TRUNC | O_CREAT, 0644);
                                else fd = open(comm->stdout, O_WRONLY | O_CREAT, 0644);

                                if (fd < 0) { perror("open"); exit(1); }
                                lseek(fd, 0, SEEK_END);
                                if (dup2(fd, 1) != 1) {   
                                    perror("error dup2(fd)");
                                    exit(1);
                                }
                                close(fd);
                            }   
                        }
                        else {
                            //redirect standard output to pipe
                            if (dup2(pipefd[1], 1) != 1) {
                                perror("dup2(pipefd[1])");
                                exit(1);
                            }
                        }
                        close(pipefd[1]);

                        if(i == 0) {
                            if(comm->stdin != NULL) {
                                
                                fd = open(comm->stdin, O_RDONLY, 0644);
                                if (fd < 0) { perror("open"); exit(1); }
                                if (dup2(fd, 0) != 0) {   
                                    perror("dup2(fd)");
                                    exit(1);
                                }
                                close(fd);
                                
                            }
                        }
                        else {
                            
                            if (dup2(prev, 0) != 0) {   
                                perror("dup2(prev)");
                                exit(1);
                            }
                            close(prev);
                            
                        }
                        close(pipefd[0]);
                        
                        //finally exec
                        //fprintf(stderr, "Execcing\n");
                        
                        execvp(comm->argvs[i][0], comm->argvs[i]);
                        s = (char *) malloc((strlen(comm->argvs[i][0]) + 1) * sizeof(char));
                        sprintf (s, "%s", comm->argvs[i][0]);
                        perror(s);
                        exit(1);

                        
                    }
                    else {
                        if(i == comm->n_commands - 1) {
                            close(pipefd[0]);
                        }
                        close(pipefd[1]);

                        if(i == 0) {
                            prev = pipefd[0];
                        }
                        else {
                            if(prev > 0) close(prev);
                            prev = pipefd[0];
                        }
                        jrb_insert_int(t, j, new_jval_v(NULL));
                    }     
                }
                
                
                if(comm->wait == 1) {
                    i = 1;
                    while(i) {
                        j = wait(&dummy1);
                        t_tmp = jrb_find_int(t,j);
                        //printf("%d\n", t_tmp->key.i);
                        if(t_tmp != NULL) {
                            jrb_delete_node(t_tmp);
                        }
                        if(jrb_empty(t)) i = 0;
                    }

                    /*
                    jrb_traverse(t_tmp, t) {
                        printf("%d", t_tmp->key.i);
                    }
                    */
                }

                //wait(&dummy1);
                //wait(&dummy2);
                //if (WEXITSTATUS(dummy1) || WEXITSTATUS(dummy2)) {
                //fprintf(stderr, "Abnormal exit.\n");
                //}
                free_command(comm);

            }
        }

    
    }
}