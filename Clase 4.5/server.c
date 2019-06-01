#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

#define PORT 3535                                                              //Puerto a usar, no tiene que ser el mismo de navegacion, etc parameter
#define BACKLOG 2                                                              //Determina la cantidad de usuarios que se pueden conectar simultaneamente
size_t tama1;
size_t tamaClient;
int r, fd, fd1, opt = 1;                                                                          //Evitar errores
struct sockaddr_in server;
struct sockaddr_in client1;

int main(int argc, char const *argv[]) {
  fd = socket(AF_INET,SOCK_STREAM,0);                                           //Instanciar el socket
                                                                                //AF_INET hace referencia al protocolo IPV4 que se va a usar, estos
                                                                                // argumentos estan definidos en el MAN
                                                                                //Argumentos de la estructura socket (Que es del tipo SOCKADDR_IN)
  if (fd < 0)
  {
    perror("Error socket");
    exit(EXIT_FAILURE); 
  }
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
  } 

  server.sin_family = AF_INET;
  server.sin_port = htons( PORT );                                              //Buscar info de esta funcion
  server.sin_addr.s_addr = INADDR_ANY;
  bzero(server.sin_zero,8);                                                     //Este ultimo dato de la uncion siempre es cero//Buscar info de bzero()
  tama1 = sizeof(struct sockaddr_in);
  r = bind(fd,(struct sockaddr*)&server, tama1);
  //Validacion r = -1;
  if ( r < 0)
  {
    perror("Error bind");
    exit(EXIT_FAILURE); 
  }
  r = listen(fd, BACKLOG);
  //Se valida igualmente
  if (r < 0)
  {
    perror("Error listen");
    exit(EXIT_FAILURE); 
  }
  tamaClient = 0;
  fd1 = accept(fd,(struct sockaddr*)&client1,(socklen_t *)&tamaClient);
  //Validar error
  if (fd1 < 0)
  {
    perror("Error accept");
    exit(EXIT_FAILURE); 
  }
  
  r = send(fd1,"Hola mundo",10,0);                                                //Se envia en fd1 pues es el creado para la comunicacion con el cliente
                                                                                //El "hola mundo" puede ser ocupado por un puntero donde esta la info
                                                                                //que quiero enviar (Esto apunta a memoria, no a un archivo)
  //validar
  if (r < 0)
  {
    perror("Error send");
    exit(EXIT_FAILURE); 
  }
  
  close(fd1);
  close(fd);
  return 0;
}
