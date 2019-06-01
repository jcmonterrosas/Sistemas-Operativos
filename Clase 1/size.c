#include <stdio.h>
#include <stdlib.h>


int main(){

	char *a;
	int *b;
	float *c;
	short *d;
	double *e;
	long *f;
	long long *g;
	long double *h;

	a = (char *) malloc(10);
	if(a == NULL){
		perror("error malloc");
		exit(-1);	
	}

	printf("Char: %li \n",sizeof(a));
	printf("int: %li \n",sizeof(b));
	printf("float: %li \n",sizeof(c));
	printf("short: %li \n",sizeof(d));
	printf("double: %li \n",sizeof(e));
	printf("long: %li \n",sizeof(f));
	printf("long long: %li \n",sizeof(g));
	printf("long double: %li \n",sizeof(h));

	return(0);
}