#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

char buff[256];

void Usage() {
    sprintf(buff, "./calc1 N fichero\n");
    write(1, buff, strlen(buff));
    exit(0);
}

void ErrorExit(char *msg, int exitStat) {
    perror(msg);
    exit(exitStat);
}

int main(int argc, char *argv[]) {
    if (argc != 3) Usage();
    
    int n = atoi(argv[1]);
    int pids[n];
    int dead, st;
    int i = 0;
    int stop = 0;
    int incalculable = 1; //bool

    //creamos N hijos 
    while (i < n && !stop) {
        pids[i] = fork();
        if (pids[i] < 0) ErrorExit("Error fork", -1);
        else if (pids[i] == 0) execlp("./hijo1", "./hijo1", argv[2], NULL);

        //AHORA COMPRUEVO PARA TODOS LOS HIJOS QUE YA EXISTEN
        int j = 0;
        while (dead = waitpid(pids[i], &st, WNOHANG) == 0 && j <= i) {
            ++j;
        }
        //Si dead contiene un pid (hijo acabado)
        if (dead > 0) {
            if (WIFWXITED(st)) {
                int aux = WEXITTATUS(st);
                if (aux == 0) { //acabado correctamente
                    incalculable = 0;
                    stop = 1;
                    sprintf(buff, "%d: ended correctly\n", dead);
                    write(1, buff, strlen(buff));
                    for (int j = 0; j <= i; ++j) {
                        kill(pids[i], SIGKILL);
                    }
                }
                else { //acabado raro
                    sprintf(buff, "%d: ended with exit code %d\n", dead);
                    write(1, buff, strlen(buff));
                }
            }
            else {
                sprintf(buff, "%d: ended with signal %d\n", dead, WTERMSIG(st));
            }
        }
        ++i;
    }
    while ((dead = waitpid(-1, &st, 0)) > 0) {
        if (WIFWXITED(st)) {
            int aux = WEXITTATUS(st);
            if (aux == 0) { //acabado correctamente
                incalculable = 0; //es calculabre
                stop = 1;
                sprintf(buff, "%d: ended correctly\n", dead);
                write(1, buff, strlen(buff));
                for (int i = 0; i < n; ++i) {
                    kill(pids[i], SIGKILL);
                }
            }
            else { //acabado raro
                sprintf(buff, "%d: ended with exit code %d\n", dead);
                write(1, buff, strlen(buff));
            }
        }
        else {
            sprintf(buff, "%d: ended with signal %d\n", dead, WTERMSIG(st));
        }
    }
    if (incalculable) {
        sprintf(buff, "INCALCULABLE\n");
        write(2, buff, stlen(buff));
    }
}