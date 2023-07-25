/* Rep el nom de un fitxer, un nombre natural N > 0 i un nombre natural S > 0.
El proc pare ha de llegir de l'entrada standard N caracters i crear N fills, un per cada caracter llegit.
Cada fill fa una esoera bloquejant a un SIGUSR1.

El pare envia un SIGUSR1 a tots els fills. El para fa una espera activa de S segons i llavors
enviar el SIGUSR1 al fill pertinent. Espera a tots els fills.

Quan els fills reben SIGUSR1 haura d'imprimir "Llista pos lletra aux: " on aux es el seu caracter 
corresponent. Després crida al proc aparicions i imprimeix les pos del caracter que li toca.

Cada vegada que el pare/fills rebin un signal imprimeixen pel canar estandar "PARE(PID): ALARMA REBUDA".
*/

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

//signal recibido o no
int r = 0; 

void Usage() {
    char buff[256];
    sprintf(buff, "./aparicions2 file N S\n");
    write(1, buff, strlen(buff));
    exit(0);
}

void ErrorExit(char *msg, int exitStat) {
    perror(msg);
    exit(exitStat);
}

void tratAlarm() {
    char buff[256];
    sprintf(buff, "PARE (%d): ALARMA REBUDA\n", getpid());
    write(1, buff, strlen(buff));
    r = 1;
}

void tratSigusr1() {
    char buff[256];
    sprintf(buff, "FILL (%d): SIGUSR1 REBUDA\n", getpid());
    write(1, buff, strlen(buff));
}

int main(int argc, char *argv[]) {
    char buff[256];
    if (argc != 4) Usage();

    int N = aoti(argv[1]);
    int S = atoi(argv[2]);

    //reprogramo los signals
    struct sigaction t;
    sigemptyset(&t.sa_mark);
    t.sa_flags = 0;
    //SIGALRM
    t.sa_handler = tratAlarm;
    if (sigaction(SIGALRM, &t, NULL) < 0) ErrorExit("SIGACTION", -1);
    //SIGUSR1
    t.sa_handler = tratSigusr1;
    if(sigaction(SIGUSR1, &t, NULL) < 0) ErrorExit("SIGACTION", -1);

    //Padre lee canal estandar
    char caracters[N];
    for (int i = 0; i < N, ++i) {
        read(0, &caracters[i], sizeof(char));
    }

    //Creamos N hijos
    int pids[N];
    for (int i = 0; i < N, ++i) {
        pids[i] = fork();
        if (pids[i] < 0) ErrorExit("fork", -1);
        //Hijos
        else if (pids[i] == 0) {
            //Esperamos SIGUSR1
            sigset_t mask;
            sigfillset(&mask);
            sigdelset(&mask, SIGUSR1);
            sigsuspend(SIGUSR1);

            //Vamos a aparicions
            sprintf(buff, "Llista de posicions de la lletra %c: ", caracters[i]);
            write(1, buff, strlen(buff));
            execlp("./aparicions", "./aparicions", argv[1], &caracters[i], NULL);
            ErrorExit("execlp", -1);
        }
    }

    //Padre
    for (int i = 0; i < N; ++i) {
        //Activamos alarma de S segundos
        alarm(S);
        while (!r);
        //Mando sigusr1 a mis hijos en orden
        kill(pids[i], SIGUSR1);
        //Reseteo alarma
        r = 0;
    }

    //Espero a que acaben todos mis hijos
    while (waitpid(-1, NULL, 0) > 0);
}