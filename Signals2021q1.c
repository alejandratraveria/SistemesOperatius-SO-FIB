//Control SO2021q1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void usage() {
    char buff[256];
    sprintf(buff, "./signals n (0 <= n <= 10)\n");
    write(1, buff, strlen(buff));
    exit(0);
}

void error(char *msg, int exitStatus) {
    perror(msg);
    exit(exitStatus);
}

void treatSigusr1() {
    int left = alarm(0);
    exit(left);
}

void treatAlrm(int s) {
    char buff[256];
    sprintf(buff, "%d: Timeout\n", getpid());
    write(1, buff, strlen(buff));
}

int main(int argc, char *argv[]) {
    if (argc != 2) usage();

    int n = atoi(argv[1]);
    if (n < 0 || n > 10) usage();

    int *pids = malloc(n*sizeof(int));

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    struct sigaction sa;
    sa.sa_handler = treatSigusr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = treatAlrm;
    sigaction(SIGALRM, &sa, NULL);

    for (int i = 0; i < n; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            alarm(3);

            sigfillset(&mask);
            sigdelset(&mask, SIGALRM);
            sigdelset(&mask, SIGUSR1);
            sigsuspend(&mask);

            exit(0);
        }
    }

    for (int i = 0; i < n; i+=2) {
        kill(pids[i], SIGUSR1);
    }

    int dead, st;
    while ((dead = waitpid(-1, &st, 0)) > 0) {
        if (dead%2 == 0) {
            if (WIFEXITED(st)) {
                int west = WEXITSTATUS(st);

                char buff[256];
                printf(buff, "Hijo %d: Segundos restantes %d\n", dead, west);
                write(1, buff, strlen(buff));
            }
        }
    }
    free(pids);
    exit(0);
}   
