#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(){

pid_t  p;

p = fork();

if(p < 0){
	perror("Error");
	exit(-1);
}
if(!p){
	printf("Hola soy hijo y PID = %i \n", p);
}else{
	printf("Hola soy padre y PID = %i \n", p);
}
}
