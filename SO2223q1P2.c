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
int recibido = 0;

void Usage() {
    sprintf(buff, "./p2.c x y [x y]\n");
    write(1, buff, strlen(buff));
    exit(0);
}

void ErrorExit(char *msg, int exitStat) {
    perror(msg);
    exit(exitStat);
}

void tratSigalarm() {
    recibido = 1;
}

int main(int argc, char *argv[]) {
    if (argc%2 != 0) Usage();
    if ((mknod("MIPIPE", S_IFIFO|0666, 0) < 0) && (errno != EXISTS)) {
        ErrorExit("Error mknod", -1);
    }

    //reprogramamos el sigalarm para que vaya a la funcion tratSigalarm
    struct sigaction sa;
    sa.sa_handler = tratSigalarm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) < 0) ErrorExit("Error sigaction", -1);

    int hijos = (argc-1)/2;
    for (int i = 0; i < hijos; ++i) {
        int pid = fork();
        if (pid == 0) {
            sigset_t mask; 
            sigemptyset(&mask);
            sigaddset(&mask, SIGUSR1);
            if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) ErrorExit("Error sigprokmask", -1);
            
            int fd = open("MIPIPE", O_WRONLY);
            dup2(fd,10); //queremos que lo que se escribia por el canal 10 se escriba por la pipe
            close(fd);
            execlp("./p1", "./p1", argv[1+2*i], argv[2+2*i]);
            ErrorExit("Error execlp", -1)
        }
        
        alarm(2);
        while(!recibido);
        recibido = 0;

        kill(pid, SIGUSR1);
        int n = atoi(argv[1+2*i]);
        int fd = ("MIPIPE", O_RDONLY);
        int size = 0;
        void *v;
        if (argv[2+2*i][0] == 'c') {
            size = n*sizeof(char);
            v = (char*) malloc(size);
            int j = 0;
            while (read(fd, &v[j], sizeof(char)) > 0) ++j;
        }
        else {
            size = n*sizeof(int);
            v = (int*) malloc(size);
            int j = 0;
            while (read(fd, &v[j], sizeof(int)) > 0) ++j;
        }
        lose(fd);
        waitpid(pid,NULL,0);
        spintf(buff, "salida-%d.dat", pid);
        int fdsal = open(buff, O_WRONLY | O_TRUNC | O_CREAT, 0,660);
        write(fdsal, &v, size);
        free(v);
    }
    sprintf(buff, "Final proceso padre\n");
    write(1, &buff, strlen(buff));
}
