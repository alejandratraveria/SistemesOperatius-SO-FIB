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
    sprintf(buff, "./p4 N semilla\n");
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
    int exit = 0;

    int p[2];
    //p[0] LECTURA
    //p[1] ESCRITURA
    pipe(p);

    for (int i = 0; i < n; ++i) {
        int pid = fork();
        if (pid < 0) ErrorExit("Error fork", -1);
        else if (pid == 0) {//soy hijo
            dup2(p[1], 1); //redirijo canal de escritura standard a la pipe
            close(p[0]);
            close(p[1]);
            if (i == 0) {
                execlp("./dummy2", "./dummy2", argv[2], NULL);
                ErrorExit("Error execlp", -1);
            }
            else {
                sprintf(buff, "%d", semilla);
                execlp("./dummy2", "./dummy2", buff; NULL);
                ErrorExit("Error execlp", -1);
            }
        }
        int st; 
        waitpid(pid, &st, 0);
        if (WIFEXITED(st)) exit = WEXITSATUS(st);
        semilla += exit;
    }
    int pid2 = fork();
    int x;
    int total = 0;
    if (pid2 < 0) ErrorExit("Error fork", -1);
    else if (pid2 == 0) {
        close(p[1]);
        while (read(p[0], &x, sizeof(int)) > 0) {
            total += x;
        }
        sprintf(buff, "El total de la pipe %d\n", total);
        write(1, buff, strlen(buff));
        exit(0);
    }
    close(p[0]);
    close(p[1]);
    sprintf(buff, "El total es %d\n", semilla);
    write(1, buff, strlen(buff));
    exit(0)
}