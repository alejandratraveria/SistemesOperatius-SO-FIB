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
    sprintf(buff, "./busqueda_parcial_espera arg1 arg2 arg3\n");
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
    if(argc != 4) Usage();

    argPid = atoi(argv[1]);

    struct sigaction sa;
    sa.sa_handler = treatment;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGUSR1, &sa, NULL) < 0) ErrorAndExit("sigaction SIGUSR1", -1);
    if(sigaction(SIGUSR2, &sa, NULL) < 0) ErrorAndExit("sigaction SIGUSR2", -1);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigsuspend(&mask);

    char buff[256];
    int pid = fork();
    if (pid == 0) {
        execlp("grep", "grep", argv[2], argv[3], NULL);
        ErrorAndExit("execlp", -1);
    }
    else if (pid < 0) ErrorAndExit("fork", -1);

    int st;
    waitpid(pid, &st, 0);
    if(WIFEXITED(st)) {
        int west = WEXITSTATUS(st); 
        sprintf(buff, "Termina grep %s %s: exit %d\n", argv[2], argv[3], west);
        write(1, buff, strlen(buff));
        if (argPid != 0) {
            if (west == 1) kill(argPid, SIGUSR1);
            else if (west == 0) kill(argPid, SIGUSR2);
        }
    }
}