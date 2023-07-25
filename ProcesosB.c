//Control SO1920q1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


void usage() {
    char buff[256];
    sprintf(buff, "Usage: ./status [PID]\n");
    write(1, buff, strlen(buff));
}

void errorAndExit(char *s, int error) {
    perror(s);
    exit(error);
} 

int main(int argc, char *argv[]) {
    if (argc > 11) usage();
    
    int n = argc-1;
    char buff[256];
    //como podemos crear el vector en memoria dinamica?
    //int *pids = sbrk(10*sizeof(int));
    //int *pids = malloc(10*sizeof(int));
    int pids[10]; //como mis hijos mutan me guardo todos sus pids
    for (int i = 0; i < n; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            sprintf(buff, "/proc/%s/status", argv[i+1]);
            execlp("grep", "grep", "State", buff, NULL);
            errorAndExit("execlp", -1);
        }
        else if (pids[i] < 0) errorAndExit("fork", -1);
    }
    int dead, st;
    while ((dead = waitpid(-1, &st, 0)) > 0) { //Espero a cualquiera de is hijos y recojo su resultado
        if(WIFEXITED(st)) {
            int west = WEXITSTATUS(st);
            if (west != 0) {
                int i = 0;
                while (pids[i] != dead) ++i;
                sprintf(buff, "Proceso %s no existe\n", argv[1+i]);
                write(1, buff, strlen(buff));
                exit(0);
            }
        }
    }
    //si hemos creado el vector en memoria dinamica luego hay que LIBERARLA
    //sbrk(-10*sizeof(int));
    //si la hemos creado con malloc -> free(pids)
    exit(0);
}