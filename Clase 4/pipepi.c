#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

double CalcularPi(long izq, long der){
	double resultado = 0.0;
	double j = (2.0*izq)-1.0;
	int sig = 1;
	for (double i = izq; i < der; i++){
		resultado += (double)(4/j)*sig;
		j += 2;
		sig *= -1;
	}
	return resultado;
}

int main(){
    int pipefd[2];
    int r; 
    char buffer[5];
    buffer[4] = 0;
    double pi = 0.0;
	double PiHijo, PiPadre;  
    pid_t pid;

    r = pipe(pipefd);
    if(r == (-1)){
        perror("Error en pipe");
        exit(-1);
    }

    pid = fork();
    if(pid < 0){
		perror("Error");
		exit(-1);
	}
	if(pid == 0){
		close(pipefd[0]);
        PiHijo = CalcularPi(1,1e09);
        r = write(pipefd[1], &PiHijo, sizeof(double));
        if (r == 0)
        {
            perror("Error write pipe");
            exit(-1);
        }
        close(pipefd[1]);
	}else{
		close(pipefd[1]);
        PiPadre = CalcularPi(1e09+1, 2e09);
        r = read(pipefd[0], &pi, sizeof(double));
        if (r == 0)
        {
            perror("Error read pipe");
            exit(-1);
        }
        close(pipefd[0]);
        pi -= PiPadre;
        printf("Valor de Pi %1.40f \n", pi); 
	}
    exit(0);
}