//Control SO 2122q1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void Usage() {
    char buff[256];
    sprintf(buff, "./jerarquia1 fichero[1...10]\n");
    write(1, buff, strlen(buff));
}

void ErrorAndExit(char *msg, int exitStatus) {
    perror(msg);
    exit(exitStatus);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 11) Usage();

    int n = argc - 1;
    char buff[256];
    int *pids = sbrk(n*sizeof(int));
    for (int i = 0; i < n; ++i) {
        pids[i] = fork();
        if (pids[i] == 0) {
            sprintf(buff, "%s\n", argv[i+1]);
            write(1, buff, strlen(buff));
            exit(i);
        }
        else if (pids[i] < 0) ErrorAndExit("fork", -1);
    }
    int st;
    for (int i = 0; i < n; ++i) {
        waitpid(pids[i], &st, 0);
        if (WIFEXITED(st)) {
            int west = WEXITSTATUS(st);
            sprintf(buff, "Ha muerto el hijo %d con exit %d\n", pids[i], west);
            write(1, buff, strlen(buff));
        }
    }
    sbrk(-n*sizeof(int));
    exit(0);
}