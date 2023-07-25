//Control SO1920q1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void usage() {
    char buff[256];
    sprintf(buff, "Usage: ./status [PID]\n");
    write(1, buff, strlen(buff));
}

void errorAndExit(char *s, int error) {
    perror(s);
    exit(error);
} 

int main(int argc, char *argv[]) {
    if (argc == 1) usage();
    
    int n = argc-1;
    char buff[256];
    for (int i = 0; i < n; ++i) {
        int pid = fork();
        if (pid == 0) {
            sprintf(buff, "/proc/%s/status", argv[i+1]);
            execlp("grep", "grep", "State", buff, NULL);
            errorAndExit("execlp", -1);
        }
        else if (pid < 0) errorAndExit("fork", -1);
    }
}