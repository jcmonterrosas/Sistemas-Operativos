#include<stdio.h>

#include<stdlib.h>

using namespace std;

struct dataType {

	char nombre [32];
	
	short edad;

	float estatura;

	float peso;

};

struct dataType *datos;


int main() {

	
	datos = ( dataType* ) malloc( sizeof( struct dataType ) );

	if( datos == NULL ){

		perror( "Error en malloc!" );

		exit( -1 );

	}

	FILE *f;

	f = fopen( "datos.dat", "r" );

	if( f == NULL ){

		perror( "Error abriendo!" );
	
		exit( -1 );	
	
	}

	int r;

	r = fread( datos, sizeof( struct dataType ), 1, f );

	if( r == 0 ){

		perror( "Error leyendo!" );
	
		exit( -1 );	
	
	}

	printf( "%s\n", datos -> nombre );

	printf( "%hi\n", datos -> edad );

	printf( "%f\n", datos -> estatura );

	printf( "%f\n", datos -> peso );

	fclose( f );

	free( datos );
	
	return 0;

}



