//Control SO2223q1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int argPid = 0;

void Usage() {
    char buff[256];
    sprintf(buff, "./busqueda_multiple palabra ficheros[2...9]]\n");
    write(1, buff, strlen(buff));
}

void ErrorAndExit(char *msg, int exitStatus) {
    perror(msg);
    exit(exitStatus);
}

void treatment(int s) {
    char buff[256];
    sprintf(buff, "Signal recibido: %d\n", s);
    write(1, buff, strlen(buff));
    
    if (s == SIGUSR2) if (argPid != 0) kill(argPid, SIGUSR2);
} 

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 11) Usage();
    
    char buff[256];
    int n = argc - 2;
    int *pids = sbrk(n*sizeof(int));

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    for (int i = 0; i < n; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            if (i > 0) sprintf(buff, "%d", pids[i-1]);
            else sprintf(buff, "0");
            execlp("./busquedaParcialEspera", "./busquedaParcialEspera", buff, argv[1], argv[i+1], NULL);
        }
    }

    kill(pids[n-1], SIGUSR1);
    sbrk(-n*sizeof(int));
}