#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 8
struct lados
{
    long izq;
    long der;
};
struct lados *lado;	

double pi = 0.0;

void *funcion(void *ap){
    printf("%i", *(int *)ap);
}

void *CalcularPi(void *ap){
	double j = (2.0* (lado -> izq))-1.0;
	int sig = 1;
	for (double i = lado ->izq; i < lado->der; i++){
		pi += (double)(4/j)*sig;
		j += 2;
		sig *= -1;
    }
}

int main(int argc, char const *argv[]) {
    pthread_t idThread;
    lado = malloc(sizeof(struct lados));
    int n = N;
    lado -> izq = 1;
    lado -> der = 1;
    while (n > 0)
    {
        lado ->izq = lado -> der;
        lado ->der = 2e09/n;
        pthread_create(&idThread, NULL, CalcularPi, (void *)&lado); 
        n--;     
    }
    n = N;
    while (n > 0)
    {
        pthread_join(idThread, NULL);
        n--;   
    }  
    printf("Valor de Pi %1.40f \n", pi);  
    return 0;
}