all: client server

client: client.o
	gcc p2-dogClient.o -o p2-dogClient

client.o: p2-dogClient.c
	gcc -c p2-dogClient.c -o p2-dogClient.o

server: server.o
	gcc p2-dogServer.o -o p2-dogServer -lpthread

server.o: p2-dogServer.c
	gcc -c p2-dogServer.c -o p2-dogServer.o

clean: 
	
	rm -f *.o *.log p2-dogClient p2-dogServer 
