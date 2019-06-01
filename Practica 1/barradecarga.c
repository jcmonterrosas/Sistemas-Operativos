#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
 
void ShowBar(int Percent,int MaxSize,char* Msg)
{
    int i = 0,a = 0;
        for(i = MaxSize+strlen(Msg)+2; i>=0; --i) putchar('\b'); // vamos al principio de la barrita
 
    putchar('[');
        for(i = (Percent*MaxSize)/100; a<i; ++a) putchar('#');
        for(; a < MaxSize; ++a) putchar('-');
    putchar(']');
 
    printf(" %s",Msg);
    fflush(stdout);
}
 
int main()
{
    printf("Haciendo algo relevante .... \n\n");
    for(int i = 0; i < 100; ++i)
    {
       ShowBar(i,50,"  Completado");
       //sleep(1);
    }
 
}