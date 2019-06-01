#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

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

	pid_t p;
	double pi = 0.0;
	double PiHijo, PiPadre;  
	double *p_pi = &pi;
	FILE *f;
	int r;
	p = fork();

	if(p < 0){
		perror("Error");
		exit(-1);
	}
	if(p == 0){
		printf("Hola soy hijo y PID = %i \n", p);
		PiHijo = CalcularPi(1,1e09);
		f = fopen("data.dat","w+");
		if(f == NULL){
			perror("Error fopen");
			exit(-1);
		}
		r = fwrite(&PiHijo, sizeof(double), 1, f);
		if (r == 0){
			perror("Error fwrite");
			exit(-1);
		}
		printf("Valor de PiHijo: %1.40f \n", PiHijo);
	}else{
		wait(NULL);
		printf("Hola soy padre y PID = %i \n", p);
		PiPadre = CalcularPi(1e09+1, 2e09);		
		f = fopen("data.dat", "r");
		printf("Valor de PiPadre: %1.40f \n", PiPadre);
		if(f == NULL){
			perror("Error fopen");
			exit(-1);
		}
		r = fread(p_pi, sizeof(double), 1 , f);
		if( r == 0 ){
			perror( "Error leyendo!" );
			exit( -1 );	
		}
		pi -= PiPadre;
		printf("Valor de Pi %1.40f \n", pi);
	}
	r = fclose(f);
	if (r != 0)
	{
		perror("Error fclose");
		exit(-1);
	}
}
