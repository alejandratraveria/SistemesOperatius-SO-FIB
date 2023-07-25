#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

void Usage() {
	char buff[256];
	sprintf(buff,"Usage: ./aparicions3 textfile characters_file file_out N S\n");
	write(1,&buff,strlen(buff));
	exit(0);
}

void error_y_exit(char *s, int error) {
	perror(s);
	exit(error);
}

int main(int argc,char *argv[]){
	if(argc != 6) Usage();

	int p[2];
	pipe(p);

	int pid = fork();
    //hijo
	if(pid == 0){
		int fd = open(argv[3],O_WRONLY | O_TRUNC | O_CREAT, 0666);
		dup2(p[0],0);
		dup2(fd,1);
		close(p[0]);
		close(p[1]);
		execlp("./aparicions2", "./aparicions2",argv[1],argv[4],argv[5],NULL);
	}
	//padre
    int fdchar = open(argv[2],O_RDONLY);
	char c;
	close(p[0]);
	while(read(fdchar,&c,sizeof(char)) > 0){
		write(p[1],&c,sizeof(char));
	}
	close(p[1]);
	waitpid(-1,NULL, 0);
}