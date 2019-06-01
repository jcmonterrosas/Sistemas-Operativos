#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

#define PORT 3535                                                              //Puerto a usar, no tiene que ser el mismo de navegacion, etc parameter
size_t tama1;
size_t tamaClient;
int r, fd, fd1, opt = 1;                                                                          //Evitar errores
struct sockaddr_in server;
struct sockaddr_in client1;
char buffer[1024] = {0}; 

int main(int argc, char const *argv[]) {
  fd = socket(AF_INET,SOCK_STREAM,0);
  //Validar
  if (fd < 0)
  {
    perror("Error socket");
    exit(EXIT_FAILURE); 
  }
  
  server.sin_family=AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = inet_addr("127.0.0.1");                              //Con la IP publica del servidor
  bzero(server.sin_zero,8);
  tama1 = sizeof(struct sockaddr_in);                                            
  r = connect(fd,(struct sockaddr*)&server,tama1);
  //Validar
  if (r < 0)
  {
    perror("Error connect");
    exit(EXIT_FAILURE);
  }
  
  r = recv(fd,buffer,10,0);                                                     //Va a recivir un maximo de 10 bytes
  if (r < 0)
  {
    perror("Error recv");
    exit(EXIT_FAILURE);
  }
  
  printf("%s \n",buffer);
  close(fd);
  return 0;
}
