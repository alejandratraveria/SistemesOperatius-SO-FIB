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
    sprintf(buff, "./p3 N semilla segs fichero\n");
    write(1, buff, strlen(buff));
}

void ErrorExit(char *msg, int exitStat) {
    perror(msg);
    exit(exitStat);
}

void tartSigalrm() {
    sprintf(buff, "Tiempo Limite\n");
    write(1, buff, strlen(buff));
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 5) Usage();  
    
    //reconfiguramos el SIGALRM
    sigset_t mask;
    sigfillset(&mask);
    if ((sigprocmask(SIG_SETMASK, &mask, NULL)) < 0) ErrorExit("Error sigprocmask", -1);

    struct sigaction t;
    t.sa_handler = tratSigalrm;
    t.sa_flags = 0;
    sigemptyset(&t.sa_mask);
    if ((sigaction(SIGALRM, &t, NULL)) < 0) ErrorExit("Error sigaction", -1);

    sigdelset(&mask, SIGALRM);
    if((sigprocmask(SIG_UNBLOCK, &mask, NULL)) < 0) ErrorExit("Error sigprocmaks", -1);

    int n = atoi(argv[1]);
    int semilla = atoi(argv[2]);
    int segs = atoi(argv[3]);
    int exit = 0;

    int fd = open(argv[4], O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if (fd < 0) ErrorExit("Error open", -1);
    dup2(fd,1);
    close(fd);
    alarm(segs);
    
    for (int i = 0; i < n; ++i) {
        int pid = fork();
        if (pid < 0) ErrorExit("Error fork", -1);
        else if (pid == 0) {//soy hijo
            if (i == 0) {
                execlp("./dummy", "./dummy", argv[2], NULL);
                ErrorExit("Error execlp", -1);
            }
            else {
                semilla += exit;
                sprintf(buff, "%d", semilla);
                execlp("./dummy", "./dummy", buff; NULL);
            }
        }
        int st; 
        waitpid(pid, &st, 0);
        if (WIFEXITED(st)) exit = WEXITSATUS(st);
    }
    sprintf(buff, "El total es %d\n", semilla);
    write(1, buff, strlen(buff));
    exit(0);
}