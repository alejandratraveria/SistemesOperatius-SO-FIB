#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>

void Usage() {
    char buff[256];
    sprintf(buff, "Uasage: ./cuentaPalabras file\n");
    write(1,buff, strlen(buff));
    exit(0);
}

void ErrorExit(char *msg, int exitStatus) {
    perror(s);
    exit(exitStatus);
}

int main(int argc, char *argv[]) {
    if(argc != 2) Usage();
    int p[2];
    pipe(p);

    int pid1 = fork();
    if (pid1 < 0) ErrorExit("fork", -1);
    if (pid1 == 0) {
        dup2(p[1], 1); //duplicamos el canal de escritura al canal 1(tty)
        //cerrar canal duplicado + el de lectura que no se usa
        close(p[0]);
        close(p[1]); 
        execlp("./inicialesv2", "./inicialesV2", argv[1], NULL);
        ErrorExit("execlp", -1);
    }

    int pid2 = fork();
    if (pid2 < 0) ErrorExit("fork", -1);
    if (pid1 == 0) {
        dup2(p[0], 0); //duplicamos el canal de lectura al canal 0(tty)
        //cerrar canal duplicado + el de escritura que no se usa
        close(p[0]);
        close(p[1]);
        execlp("wc", "wc", "-c", NULL);
        ErrorExit("execlp", -1);
    }

    //cerramos canales padre pq no los usa
    close(p[0]);
    close(p[1]);
    while (waitpid(-1, NULL, 0)) > 0; //Esperamos a que acaben los hijos
}