#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    key_t key = 123;
    int shmId; 
    double *data_p;
    pid_t pid;

    shmId = shmget(key, sizeof(double), 0600|IPC_CREAT);
    if (shmId == -1){
        perror("Error shmget");
        exit(-1);
    }
    data_p = shmat(shmId,NULL,0);
    if (*data_p == -1){
        perror("Error shmat");
        exit(-1);
    }
    pid = fork(); 
    if(pid < 0){
		perror("Error fork");
		exit(-1);
	}
	if(pid == 0){
        *data_p = 3.1415926535;
	}else{
        printf("%1.10f \n",*data_p);
	}
    wait(NULL);
    shmdt(data_p);    
    return 0;
}
