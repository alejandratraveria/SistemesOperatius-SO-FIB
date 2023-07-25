/*Programa que llegeix un archiu y busca un caracter c  y escriu al canal estandar
de sortida les posicions on hi apareix
Es pot obtenir el char fent char c = argv[2][0]
Per fer el execlp del finll  es fa execlp("./aparicions", "aparicions", argv[1], &c, NULL)*/

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

void Usage() {
    char buff[256];
    sprintf(buff, "Usage: ./aparicions file c\n");
    write(1,buff,strlen(buff));
    exit(0);
}

void ErrorExit(char *msg, int errorStat) {
    perror(msg);
    exit(errorStat);
}

int main(int argc, char *argv[]) {
    char buff[80];
    if (argc != 3) Usage();
    
    //Abrimos el file para leerlo
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) ErrorExit("open fd", -1);
    char c = argv[2][0];

    //preparamos para leer char a char
    int pos = 0;
    char aux; 
    while (read(fd, &aux, sizeof(char)) > 0) { 
        //si el read == 0 significa que ya ha acabado de leer
        if (aux == c) {
            sprintf(buff, "%d\n", pos);
            write(1, buff, strlen(buff));
        }
        ++pos;
    }
}
