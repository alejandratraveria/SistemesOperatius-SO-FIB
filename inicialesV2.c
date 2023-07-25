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
    sprintf(buff, "Uasage: ./iniciales file\n");
    write(1,buff, strlen(buff));
    exit(0);
}

void ErrorExit(char *msg, int exitStatus) {
    perror(s);
    exit(exitStatus);
}

int main(int argc, char *argv[]) {
    if (argc != 2) Usage();
    char buff[128];
    int fd = open(argv[1], O_RDONLY); //abrimos canal de lectura
    if (fd < 0) ErrorExit("open", -1);
    sprintf(buff, "%siniciales.txt", argv[1]);
    int fd2 = open(buff, O_WRONLY | O_TRUNC | O_CREAT, 0666); //canal de escritura
    if (fd2 < 0) ErrorExit("open", -1);
    int n = read(fd, buff, sizeof(buff));
    while (n > 0) {
        char ant = buff[0];
        if (ant != ' ') write(1, ant, sizeof(char));
        for (int i = 0; i < n; ++i) {
            if (ant == ' ' && buff[i] != ' ') write(1, buff[i], sizeof(char));
            ant = buff[i];
        }
        n = read(fd, buff, sizeof(buff));
    }
}