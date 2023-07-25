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
    sprintf(buff, "Uasage: ./seleccionaNums file1 file2\n");
    write(1,buff, strlen(buff));
    exit(0);
}

void ErrorExit(char *msg, int exitStatus) {
    perror(s);
    exit(exitStatus);
}

int main(int argc, char *argv[]) {
    if (argc != 3) Usage();
    int fd = open(argv[1], O_RDONLY);
    int fd2 = open(argc[2], O_RDONLY);
    int pos, n;
    int sizeF1 = lseek(fdnums, 0, SEEK_END); //Miramos el tamaño del archivo file1

    while (read(fd2, pos, sizeod(int)) > 0) {
        read(fd2, n, sizeof(int));
        char buff[256];
        if (pos+n > sizeF1 || pos < 0 || n < 0) {
            sprintf(buff, "Error: Posicion(%d) o tamaño(%d) invalidos\n", pos, n);
            write(1, buff, strlen(buff));
        }
        else {
            lseek(fd, pos, SEEK_SET); //buscamos en el fichero file1 la pos y ponemos un puntero ahí
            int x;
            for (int i = 0; i < n; ++i) {
                read(fd, x, sizeof(int));
                sprintf(buff, "%d", x);
                write(1, buff, strlen(buff));
            }
        }
        printf("\n");
    }
}