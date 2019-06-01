#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int r;

void *funcion(void *ap){
    printf("%i", *(int *)ap);
}

int main(int argc, char const *argv[]) {
    pthread_t idThread;
    int arg = 1;
    pthread_create(&idThread, NULL, funcion, (void *)&arg);    //->POSIX / No bloqueante, los hilos se encolan, se admite un
                                                                // solo argumento y solo como un apuntador, si se quiere pasar
                                                                //muchos argumentos, crear una estructura, con todos los argumentos
                                                                //y enviar el apuntador de esa estructura 
    // si un procesos se termino, los hilos se matan
    pthread_join(idThread, NULL);   
    return 0;
}