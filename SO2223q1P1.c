#include <sdtio.h>
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
    sprintf(buff, "./p1.c N (type)\n");
    write(1, buff, strlen(buff));
    exit(0);
}

void ErrorExit(char *msg, int exitStat) {
    perror(msg);
    exit(exitStat);
}

void tratSIGUSR1() {
    sprintf(buff, "El proceso %d comineza\n", getpid());
    write(1, buff, strlen(buff));
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) Usage();
    int n = atoi(argv[1]);

    int esChar = 1; //supongo que me pasan por dflt un char
    if (argc == 3 && argv[2][0] == 'i') esChar = 0; //es un int
    if (argc == 2) {
        if (strcmp(argv[0], "./p1-integer.exe") == 0) esChar = 0;
        else if (strcmp(argv[0], "./p1-char.exe") != 0) Usage();
    }
    //reprogramamos el SIGUSR1
    struct sigaction t;
    t.sa_handler = tratSIGUSR1;
    t.sa_flags = 0;
    sigemptyset(&t.sa_mask);
    sigaction(SIGUSR1, &t, NULL);

    //hacemos la espera activa
    sigset_t mask;
    sigfillset(&mark);
    sigdelset(&mask, SIGUSR1);
    sigsuspend(&mask);

    for (int i = 0; i < n; ++i) {
        if (esChar) {
            sprintf(buff, "%d", i);
            write(1, buff, strlen(buff));
        }
        else write(10, &i, sizeof(int));
    }

    sprintf(buff, "El proceso %d ha terminado\n", getpid());
    write(1, buff, strlen(buff));
}