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

int cargar( void *ap );

int guardar( void *ap );

int main() {

	
	datos = ( dataType* ) malloc( sizeof( struct dataType ) );

	if( datos == NULL ){

		perror( "Error en malloc!" );

		exit( -1 );

	}

	cargar( datos );

	guardar( datos );

	free( datos );
	
	return 0;

}

int cargar( void *ap ){

	struct dataType *dato;

	dato = ( dataType* ) ap;

	printf( "Nombre:" );

	scanf( " %s", dato -> nombre );

	printf( "Edad:" );

	scanf( " %hi", &dato -> edad );

	printf( "Estatura:" );

	scanf( " %f", &dato -> estatura );

	printf( "Peso:" );

	scanf( " %f", &dato -> peso );

	return 0;

}

int guardar( void *ap ){

	struct dataType *dato;

	dato = ( dataType* ) ap;

	FILE *f;

	f = fopen( "datos.dat", "w+" );

	if( f == NULL ){

		perror( "Error abriendo" );
	
		exit( -1 );	
	
	}

	int r;

	r = fwrite( dato, sizeof( struct dataType ), 1, f);

	if( r == 0 ){

		perror( "Error escribiendo" );

		exit( -1 );

	}

	fclose( f );
	
	return 0;	

}
