//Control SO1920q1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int recived = 0;

void usage() {
    char buff[256];
    sprintf(buff, "Usage: ./signals N\n");
    write(1, buff, strlen(buff));
    exit(0);
}

void errorAndExit(char *msg, int exitStatus) {
    perror(msg);
    exit(exitStatus);
}

void treatment(int s) {
    if (s == SIGUSR1) recived = 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) usage();

    int n = atoi(argv[1]);
    char buff[256];
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if ((sigprocmask(SIG_BLOCK, &mask, NULL)) < 0) errorAndExit("sigprocmask", -1);

    struct sigaction sa;
    sa.sa_handler = treatment;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if ((sigaction(SIGUSR1, &sa, NULL)) < 0) errorAndExit("sigaction", -1);
    if ((sigaction(SIGALRM, &sa, NULL)) < 0) errorAndExit("sigacyion", -1);

    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    for (int i = 0; i < n; ++i) {
        int pid = fork();
        if (pid == 0) {
            sprintf(buff, "Hijo %d: %d\n", i, getpid());
            alarm(1);
            while (!recived);
            if ((kill(getppid(), SIGUSR1)) < 0) errorAndExit("kill", -1);
            exit(0);
        }
        if((sigsuspend(&mask)) < 0) errorAndExit("sigsuspend", -1);
    }
}