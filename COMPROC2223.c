#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

void ErrorExit(char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

void Usage()
{
	char buffer[256];
	sprintf(buffer, "./userprocs username\n");
	write(1, buffer, strlen(buffer));
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{

	int ps2grep[2], grep2wc[2]; //creamos las 2 pipes
	int read_wc2parent, write_wc2parent;
	int ret;
	char wcbuffer[256];
	char buffer[500];

	if (argc != 2) Usage();

	if (pipe(ps2grep) < 0) ErrorExit("pipe ps2grep");
	if (pipe(grep2wc) < 0) ErrorExit("pipe grep2wd");

	// Creo la NAMED-PIPE, control de EEXIST. Esta pipe no se usa aun asi que aun no la abrimos
	ret = mknod("wc2parent.fifo", S_IFIFO | 0666, 0);	
	if ((ret == -1) && (errno != EEXIST)) ErrorExit("Creando la named pipe");

	//HIJO 1 -> ps2grep
	switch (fork())	{	//fork para mutar a ps
		case -1:
			error_y_exit("Fork para ps");
			break;
		case 0:
			//redirecció Salida Estandar a pipe. Cierre canal duplicado.
			if (dup2(ps2grep[1], 1) < 0) ErrorExit("dup2 ps2grep[1]");
			close(ps2grep[1]);

			//cierro canales de pipe que no uso
			//evitando bloqueos
			close(ps2grep[0]);	
			close(grep2wc[0]);	
			close(grep2wc[1]);

			execlp("ps", "ps", "-eaf", (char *)NULL);
			error_y_exit("Mutación a ps");
			break;
	}

	//ps2grep -> HIJO 2 -> grep2wc 
	switch (fork())	{	//fork para mutar a grep
		case -1:
			error_y_exit("Fork para grep");
			break;
		case 0:
			//redirecció entrada Estandar a pipe. Cierre canal duplicado.
			if (dup2(ps2grep[0], 0)<0) error_y_exit("dup2 ps2grep[0]");
			close(ps2grep[0]);	

			//redirecció Salida Estandar a pipe. Cierre canal duplicado.
			if(dup2(grep2wc[1], 1)<0) error_y_exit("dup2 grep2wc[1]");
			close(grep2wc[1]);	

			//cierro canales de pipe que no uso
			//evitando bloqueos
			close(ps2grep[1]);	
			close(grep2wc[0]);	

			execlp("grep", "grep", argv[1], (char *)NULL);
			error_y_exit("Mutación a grep");
			break;
	}

	//grep2wc -> HIJO 3 -> wc2parent
	switch (fork())	{	//fork para mutar a wc
		case -1:
			error_y_exit("Fork para wc");
			break;
		case 0:
			//redirecció entrada Estandar a pipe. Cierre canal duplicado.
			if(dup2(grep2wc[0], 0)<0) error_y_exit("dup2 grep2wc[0]");
			close(grep2wc[0]);	

			//redirecció Salida Estandar a named pipe. Cierre canal duplicado. Bloqueo de open
			if((write_wc2parent = open("wc2parent.fifo", O_WRONLY, 0))<0) error_y_exit("open name pipe write");
			if (dup2(write_wc2parent, 1)<0) error_y_exit("dup2 namedpipe write");
			close(write_wc2parent);	

			//cierro canales de pipe que no uso
			//evitando bloqueos
			close(ps2grep[0]);
			close(ps2grep[1]);	
			close(grep2wc[1]);	

			execlp("wc", "wc", "-l", (char *)NULL);
			error_y_exit("Mutación a wc");
			break;
	}

	//PADRE
	//Cierro los canales de pipe que no uso
	close(grep2wc[0]);
	close(grep2wc[1]);
	close(ps2grep[0]);
	close(ps2grep[1]);

	//abre la named-pipe. Hay bloqueo
	if((read_wc2parent = open("wc2parent.fifo", O_RDONLY, 0))<0) error_y_exit("open named pipe read");	

	int valor;
	//la lectura se bloquea hasta que wc acaba 
	while ((ret = read(read_wc2parent, wcbuffer, strlen(wcbuffer))) > 0)	{
		//nos guardamos el valor para escribirlo en el fichero
		valor = atoi(wcbuffer);	
		//Quitamos el enter de la ultima pos del buffer
		wcbuffer[ret-1] = '\0'; 
		sprintf(buffer, "El usuario %s está ejecutando %s procesos\n", argv[1], wcbuffer);
		write(1, buffer, strlen(buffer));
	}
	//cerranos el canal de lectura de la pipe con nombre 
	//El de escritura se ha cerrado cuando el H3 ha acabado de escribir
	close(read_wc2parent);
	while (waitpid(-1, NULL, 0) > 0) ;

	//apertura/creación del fichero
	char nfich[256], pregunta[1000], respuesta[80];
	
	//nombre del fichero
	sprintf(nfich, "%s.dat", argv[1]);	
	//pregunta por existencia O_CREAT|O__EXCL
	int fichero = open(nfich, O_WRONLY | O_CREAT | O_EXCL, 0644);  
	
	if (fichero == -1) { //si no se puede abrir
		if (errno != EEXIST) ErrorExit("crear fichero");
		else {  //el fichero previamente existe
			ret = sprintf(pregunta, "\n!!!! El fichero ya existe: Pulsa S + return para SOBREESCRIBIRLO, \ncualquier otra cosa para + return para AÑADIR los datos al final:  ");
			write(1, pregunta, ret);
			read(0, respuesta, 80);
			if (strcmp(respuesta, "S\n") == 0) {
				// Sobre escritura del fichero O_CREAT|O_TRUNC
				fichero = open(nfich, O_WRONLY | O_CREAT | O_TRUNC,0644); 
			} 
			else {
				//apertura normal para añadir datos al final
				fichero = open(nfich, O_WRONLY, 0644); 
			}
		}
	}
	//ponemos el puntero de L/E al final del fichero
	lseek(fichero, 0, SEEK_END);  
	write(fichero, &valor, sizeof(int));
	close(fichero);
}
