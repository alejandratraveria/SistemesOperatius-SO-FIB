//Control SO2223q1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

void Usage() {
    char buff[256];
    sprintf(buff, "./busqueda_parcial arg1 arg2 arg3\n");
    write(1, buff, strlen(buff));
}

void ErrorAndExit(char *msg, int exitStatus) {
    perror(msg);
    exit(exitStatus);
}

int main(int argc, char *argv[]) {
    if(argc != 4) Usage();

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
    }
}