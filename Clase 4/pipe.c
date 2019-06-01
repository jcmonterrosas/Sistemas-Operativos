#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int pipefd[2];
    int r; 
    char buffer[5];
    buffer[4] = 0;
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
        r = write(pipefd[1], "hola", 5);
        if (r == 0)
        {
            perror("Error write pipe");
            exit(-1);
        }
        close(pipefd[1]);
	}else{
		close(pipefd[1]);
        r = read(pipefd[0],buffer,5);
        if (r == 0)
        {
            perror("Error read pipe");
            exit(-1);
        }
        close(pipefd[0]);
        printf("%s \n", buffer);   
	}
    exit(0);
}