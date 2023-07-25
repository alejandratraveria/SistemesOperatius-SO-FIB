#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

char buff[256];

void Usage() {
    sprintf(buff, "./p1 N semilla\n");
    write(1, buff, strlen(buff));
}

void ErrorExit(char *msg, int exitStat) {
    perror(msg);
    exit(exitStat);
}

int main(int argc, char *argv[]) {
    if (argc != 3) Usage();

    int n = atoi(argv[1]);
    int semilla = atoi(argv[2]);
    int ex = 0;

    for (int i = 0; i < n; ++i) {
        int pid = fork();
        if (pid < 0) ErrorExit("Error fork", -1);
        else if (pid == 0) {//soy hijo
            if (i == 0) {
                execlp("./dummy", "./dummy", argv[2], NULL);
                ErrorExit("Error execlp", -1);
            }
            else {
                sprintf(buff, "%d", semilla);
                execlp("./dummy", "./dummy", buff, NULL);
            }
        }
        int st; 
        waitpid(pid, &st, 0);
        if (WIFEXITED(st)) ex = WEXITSTATUS(st);
        semilla += ex;
    }
    sprintf(buff, "El total es %d\n", semilla);
    write(1, buff, strlen(buff));
    exit(0);
}