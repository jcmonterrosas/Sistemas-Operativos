#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

//Comentario de test                                                            
FILE *fp;
FILE *dogNames; 
char opcion;
int i = 0, j = 0;
long positionReg = 0, block = 0, nregisters = 0;
char buffer[50];
char * nombre;
size_t tama1;
int r, socket_servidor, socket_cliente, opt = 1;                                                                          //Evitar errores
struct sockaddr_in server;
struct sockaddr_in client;
time_t t;
socklen_t sin_size;

#define SIZE_HASH 2000
#define NAME_FILE "dataDogs.dat"
#define PORT 3535                                                              
#define BACKLOG 4 
//Estructuras y variables
struct dogType{
    char nombre[32];
    int edad;
    char raza[16];
    int estatura;
    float peso;
    char genero;
};
char razas[][32] = {

    "Boxer", "Labrador", "Huskee", "Poodle", "Beagle",
    "Akita", "Dalmata", "Bulldog", "Pitbull", "Bull terrier",
    "Doberman", "Pastor Alemán", "Pastor collie", "Golden retriever", "Mestizo",
    "Pug", "Rottweiler", "Chow Chow", "Bulldog francés", "Chihuahua",
    "Cocker spaniel", "Cotón de tulear", "Dogo", "Fila brasileiro", "Galgo",
    "Gran danés", "Mastín", "Pastor Belga", "Pequinés", "Pinscher",
    "Sabueso", "Samoyedo", "San bernardo", "Schnauzer", "Shar pei",
    "Shiba", "ShihTzu", "Spaniel", "Teckel", "Terranova",
    "Terrier", "Yorshire Terrier", "Pastor Ovejero", "Springer", "Spitz"
};
char genero[] = {'h', 'm', 'H', 'M'};
struct dogType *dog;												//*dog indica una pocicion de memoria, del tipo dogType
const int SIZE_DATA_DOG = sizeof(struct dogType);

//Declaracion de funciones
int seeknode(long hashnumber);
void push(struct dogType *dog);

void ingresar(void);
void ver(int modo);
void borrar(void);
void buscar(void);

void regresar(void (*src)(void), void (*dst)(void), char * message);
void continuar(void (*dst)(void));
void menu(void);

//Hilos y Socket
void enviar(int socket_cliente, void *pointer, size_t size){
        r = send(socket_cliente, pointer, size, 0);
        if(r < 1) {
                perror("send error");
                exit(-1);
        }}
void recibir(int socket_cliente, void *pointer, size_t size){
        r = recv(socket_cliente, pointer, size, 0);
        if(r < 1) {
                perror("recv error");
                exit(-1);
        }}
void crear_socket(){
        socket_servidor = socket(AF_INET, SOCK_STREAM, 0);

        if(socket_servidor == -1) {
                perror("\n\t Error en la creación del socket...");
                exit(-1);
        }
        
        if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
        } 
        bzero(server.sin_zero, 8);

        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.s_addr = INADDR_ANY;

        r = bind(socket_servidor, (struct sockaddr*) &server, sizeof(struct sockaddr_in));
        if(r == -1) {
                perror("Error al iniciar el servidor\n");
                exit(-1);
        }

        r = listen(socket_servidor, BACKLOG);
        if(r == -1) {
                perror("listen error");
                exit(-1);
        }}
//Funciones de inicio
int leer(void * data, size_t size){
    if(fread(data, size, 1, fp) <= 0)
    {
        printf("\n\tError de lectura...");
        return 0;
    }

    fflush(fp);
    return 1;}
int escribir(void * data, size_t size){
    if(fwrite(data, size, 1, fp) <= 0)
    {
        printf("\n\tError de escritura...");
        return 0;
    }
    fflush(fp);
    return 1;}

void formatear(){
    block = 0;
    j = 2 * SIZE_HASH;

    for(i = 0; i < j; i++)
    {
        fwrite(&block, 8, 1, fp);
    }
}
//Funciones de control de info
long hash(const char *str){
    long hashnumber;
    const char *ustr;

    ustr = (const char *) str;

    hashnumber = 0;

    while(*ustr != '\0')
    {
        hashnumber = (hashnumber * 256 + *ustr) % SIZE_HASH;
        ustr++;
    }

    if(hashnumber < 0)
    {
        hashnumber *= -1;
    }

    return hashnumber;}
long seekreg(long n, long hashnumber){
    long poscur;
    long posnext;

    fseek(fp, 8 * (hashnumber + SIZE_HASH), SEEK_SET);
    fread(&posnext, 8, 1, fp);

    do
    {
        poscur = posnext;
        fseek(fp, poscur + SIZE_DATA_DOG, SEEK_SET);
        fread(&posnext, 8, 1, fp);

        if(posnext == n)
        {
            n = poscur;
            break;
        }
    }
    while(posnext != 0);

    if(n != poscur)
    { return -1; }

    return n;}
void push(struct dogType *dog){
    block = 0;
    fseek(fp, 0, SEEK_END);
    positionReg = ftell(fp);
    //block = 0;

    fwrite(dog, SIZE_DATA_DOG, 1, fp);
    fwrite(&block, 8, 1, fp);

    fseek(fp, hash(dog->nombre) * 8, SEEK_SET);
    fread(&block, 8, 1, fp);

    fseek(fp, -8, SEEK_CUR);
    fwrite(&positionReg, 8, 1, fp);

    if(block == 0)
    {
        fseek(fp, (8 * SIZE_HASH) - 8, SEEK_CUR);
        fread(&block, 8, 1, fp);

        if(block == 0)
        {
            fseek(fp, -8, SEEK_CUR);
            fwrite(&positionReg, 8, 1, fp);
        }

        else
        {
            printf("\n\n\tError en nodo de inicio No. %li", hash(dog->nombre));
        }
    }

    else
    {
        fseek(fp, block + SIZE_DATA_DOG, SEEK_SET);
        fwrite(&positionReg, 8, 1, fp);
    }}
int pop(char * parameter){
    j = 0;
    long hashnumber = hash(parameter);
    //fseek(fp, 8 * (hashnumber), SEEK_SET);
    block = 0;
    fseek(fp, 8 * (hashnumber + SIZE_HASH), SEEK_SET);
    fread(&block, 8, 1, fp);

    if(block == 0)
    {
        return 0;
    }
    else
    {
        do
        {
            fseek(fp, block, SEEK_SET);
            leer(dog, SIZE_DATA_DOG);

            if(strcmp(parameter, dog->nombre) == 0)
            {
                if(j == 0)
                {
                    printf
                    (
                        "\n\n\t%s\t%s",
                        "No. registro",
                        "Mascota"
                    );
                }

                printf
                (
                    "\n\t%li\t\t%s",
                    ((block - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8)) + 1,
                    dog->nombre
                );

                j++;
            }

            fread(&block, 8, 1, fp);
        }
        while(block != 0);

        return 1;
    }}

int truncar(long n){
    void * buffer;
    char * dogName;
    long * posnext;
    long aux, tambuffer;

    dogName = malloc(32);
    posnext = malloc(8);

    n = (n - 1) * (SIZE_DATA_DOG + 8) + (SIZE_HASH * 16);

    fseek(fp, 0, SEEK_END);
    aux = ftell(fp);

    fseek(fp, n, SEEK_SET);
    leer(dogName, 32);

    fseek(fp, SIZE_DATA_DOG - 32, SEEK_CUR);
    leer(posnext, 8);

    tambuffer = aux - ftell(fp);

    if(tambuffer != 0)
    {
        buffer = malloc(tambuffer);
        leer(buffer, tambuffer);

        fseek(fp, n, SEEK_SET);
        escribir(buffer, tambuffer);

        free(buffer);
    }

    ftruncate(fileno(fp), aux - (SIZE_DATA_DOG + 8));

    //--
    long hashnumber = hash(dogName);
    free(dogName);

    fseek(fp, 8 * hashnumber, SEEK_SET);
    leer(&aux, 8);

    // Ultimo nodo
    if(aux == n)
    {
        fseek(fp, (8 * SIZE_HASH) - 8, SEEK_CUR);
        fread(&aux, 8, 1, fp);

        if(aux == n)
        {
            aux = 0;
            fseek(fp, 8 * hashnumber, SEEK_SET);
            fwrite(&aux, 8, 1, fp);

            fseek(fp, (8 * SIZE_HASH) - 8, SEEK_CUR);
            fwrite(&aux, 8, 1, fp);
        }

        else
        {
            aux = 0;
            long posback = seekreg(n, hashnumber);

            if(posback == -1)
            {   
                return 0;
                //printf("\n\n\t(NU) - Error en busqueda de registro No. %li", n);
            }

            else
            {
                fseek(fp, -8, SEEK_CUR);
                fwrite(&aux, 8, 1, fp);

                fseek(fp, 8 * hashnumber, SEEK_SET);
                fwrite(&posback, 8, 1, fp);
            }
        }
    }

    else
    {
        fseek(fp, (8 * SIZE_HASH) - 8, SEEK_CUR);
        leer(&aux, 8);

        // Primer nodo
        if(aux == n)
        {
            fseek(fp, -8, SEEK_CUR);
            fwrite(posnext, 8, 1, fp);
        }

        // Nodos intermedios
        else
        {
            long posback = seekreg(n, hashnumber);

            if(posback == -1)
            {
                return 0;
                //printf("\n\n\t(NI) - Error en busqueda de registro No. %li", n);
            }

            else
            {
                fseek(fp, -8, SEEK_CUR);
                fwrite(posnext, 8, 1, fp);
            }
        }
    }

    //---
    rewind(fp);

    for(i = 0; i < 2 * SIZE_HASH; i++)
    {
        fread(&aux, 8, 1, fp);

        if(aux > n)
        {
            aux -= (SIZE_DATA_DOG + 8);
            fseek(fp, -8, SEEK_CUR);
            fwrite(&aux, 8, 1, fp);
        }
    }

    nregisters--;
    fseek(fp, 16 * SIZE_HASH, SEEK_SET);

    for(i = 0; i < nregisters; i++)
    {
        fseek(fp, SIZE_DATA_DOG, SEEK_CUR);
        fread(&aux, 8, 1, fp);

        if(aux > n)
        {
            aux -= (SIZE_DATA_DOG + 8);
            fseek(fp, -8, SEEK_CUR);
            fwrite(&aux, 8, 1, fp);
        }
    }

    free(posnext);
    return 1;}

int numreg(){
    fseek(fp, 0, SEEK_END);
    nregisters = (ftell(fp) - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8);

    //printf("\n\tRegistros: %li", nregisters);
    enviar(socket_cliente,&nregisters,sizeof(nregisters));

    if(nregisters > 0)
    {
        //printf("\n\tIngrese número de registro:\n\t");
        //scanlidigit(10, &positionReg);
        recibir(socket_cliente,&positionReg,sizeof(positionReg));

        if(positionReg < 1 || positionReg > nregisters)
        {
            //printf("\n\n\tFuera de rango [%li max].", nregisters);
            return 0;
        }

        else
        { return 1; }
    }

    else
    {
        //printf("\n\tFallo en obtener número de registros.");
        return 0;
    }}


//Funciones de menu
void ingresar(void){
    recibir(socket_cliente, dog, SIZE_DATA_DOG);
    push(dog);
    positionReg = ((positionReg - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8)) + 1;
    enviar(socket_cliente, &positionReg, sizeof(positionReg));
    menu();}
void ver(int modo){
    if(numreg() == 1)
    {   
        block = (positionReg - 1) * (SIZE_DATA_DOG + 8) + (SIZE_HASH * 16);
        block = ((block - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8)) + 1;
        fseek(fp, (positionReg - 1) * (SIZE_DATA_DOG + 8) + (SIZE_HASH * 16), SEEK_SET);

        if(leer(dog, SIZE_DATA_DOG) == 1)
        {
            enviar(socket_cliente,dog,SIZE_DATA_DOG);
            /*printf
            (
                "\n\n\t%s \n\t%s\t\t%s \n\t%s\t\t%i \n\t%s\t\t%s \n\t%s\t%i%s \n\t%s\t\t%.2f%s \n\t%s\t\t%c \n\t%s",
                "------------ Mascota ------------",
                "Nombre:", dog->nombre,
                "Edad:", dog->edad,
                "Raza:", dog->raza,
                "Estatura:", dog->estatura, " cm(s)",
                "Peso:", dog->peso, " Kg(s)",
                "Genero:", dog->genero,
                "---------------------------------"
            );*/
        if(modo != 1){
            //Abrir registro de Mascota
            char result[40]; 
            sprintf(result, "%ld", block); 
            //printf("Hijo\n");
            char tonano[40]="gedit ";
            //intf("DTA: %s \n",tonano);
            strcat(tonano,result);//Crea una linea de comando que se ejecutará por system()
            //strcat(tonano,"&");   //
            //printf("DTA: %s \n",tonano);
            //a = sizeof(tonano);
            //printf("%i\n", a);
            system(tonano);
        }    
        }
    }
    if (modo == 0)
    {
        menu();
    }}
void borrar(void){
    ver(1);
    enviar(socket_cliente,&positionReg,sizeof(positionReg));
    //printf("\n\t%s%li%s\n\t", "¿Desea borrar el registro No. ", positionReg, "? [S/N]");
    //scanchar(1, &opcion, "SYNsyn");
    recibir(socket_cliente,&opcion,sizeof(opcion));
    if(opcion == 'n' || opcion == 'N')
    { menu();}
    else
    {
        r = truncar(positionReg);
        enviar(socket_cliente,&r,sizeof(r));
        //printf("\n\tRegistro borrado satisfactoriamente");
        menu();
    }}
void buscar(void){
    system("clear");
    char * dogName;
    dogName = malloc(32);
    dogName[32] = '\0';

    printf("\n\t--- Búsqueda ---");
    printf("\n\tNombre de la mascota: ");
    //scanletter(32, dogName);
    pop(dogName);
    if(j < 1){
        printf("\n\t%s%s%s\n", "No existe registro de la mascota \"", dogName, "\"...");
    }

    free(dogName);
    }
void menu(){
        recibir(socket_cliente, &opcion, sizeof(opcion));
        system("clear");
        switch(opcion){
            case '1':        ingresar();	break;
            case '2':        ver(0);		break;
            case '3':        borrar();		break;
            case '4':        buscar();		break;
        }}

//Main
void main(void){
    //Variables
    fp = fopen(NAME_FILE, "rb+");
    char *valor_devuelto;
    nombre = malloc(32);
    dog = malloc(sizeof(struct dogType));
    //Conexion.
    crear_socket();
    //Formateo y configuracion de informacion
    if(fp == NULL){																										
        fp = fopen(NAME_FILE, "w");									
        freopen(NAME_FILE, "rb+", fp);								
        formatear();												
        FILE *petNames;										
        petNames = fopen("nombresMascotas.dat" , "rb+");
        if(petNames == NULL){
            printf("\n\n\tEl archivo\"%s\"no existe.","nombresMascotas.dat");
        }else{
            int k;
            time_t t;
            srand((unsigned) time(&t));
            char * dogName;
            dogName = malloc(32);
            for(k = 0; k < 10000000; k++){
                fseek(petNames, ((rand() % 1716) * 32), SEEK_SET);
                fgets(dogName, 32, petNames);
                strcpy(dog->nombre, dogName);
                dog->edad = (rand() % 19) + 1;
                strcpy(dog->raza, razas[rand() % 44]);
                dog->estatura = (rand() % 19) + 1;
                dog->peso = (rand() % 123) + 1;
                dog->genero = genero[rand() % 3];
                push(dog);
            }
            free(dogName);
        }
    fclose(petNames);
    }
    //Espera conexion 
    //while(1){		
        socket_cliente = accept(socket_servidor, (struct sockaddr * )&client,&sin_size);
        if(socket_cliente == -1){
            perror("\n\t Error en recibir la conexión(accept)...");
            exit(EXIT_FAILURE);
        }
        menu();   
    //}
    close(socket_cliente);
    close(socket_servidor);	
}
