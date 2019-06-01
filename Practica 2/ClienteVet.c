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
#define PORT 3535    

//Estructuras y variables
struct dogType{
    char nombre[32];
    int edad;
    char raza[16];
    int estatura;
    float peso;
    char genero;
};    

struct sockaddr_in server;
char buffer[1024] = {0};
size_t tama1;

const int SIZE_DATA_DOG = sizeof(struct dogType);
struct dogType *dog;    //Aca se almacena una estructura tipo dogType
char genero[] = {'h', 'm', 'H', 'M'};
char opcion;
int i = 0, j = 0,r,socket_cliente;  //r y socket_cliente para manejo de la conexion
//Var registros
long positionReg = 0;   //Numero de registro
long nregisters  = 0;   //Cuantos registros hay

//Funciones
void ingresar(void);
void ver();
void borrar(void);
void buscar(void);
void salir();
void regresar(void (*src)(void), void (*dst)(void), char * message);
void continuar(void (*dst)(void));
void menu(void);
// Hilos y Socket
void enviar(void *pointer, size_t size){
        int ok = send(socket_cliente, pointer, size, 0);
        if(ok < 1) {
                perror("send error");
                exit(-1);
        }}
void recibir(void *pointer, size_t size){
        int ok = recv(socket_cliente, pointer, size, 0);
        if(ok < 1) {
                perror("recv error, conexión perdida");
                exit(-1);
        }}
void crear_socket(char *addr){

        socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in client;

        if(socket_cliente == -1) {
                perror("socket creation failed");
                exit(-1);
        }

        bzero(client.sin_zero, 8);

        client.sin_family = AF_INET;
        client.sin_port = htons(PORT);
        client.sin_addr.s_addr = inet_addr(addr);

        printf("Intentando conectar con %s en el puerto %i \n", addr, PORT);

        int ok = connect(socket_cliente, (struct sockaddr*)&client, sizeof(struct sockaddr_in));

        if(ok == -1) {
                perror("Conexion Rechazada\n");
                exit(-1);
        }else printf("Conexion Exitosa\n");}
//Main
int main(int argc, char const *argv[]) {
    system("clear");
    crear_socket((char*)argv[1]);
    menu();
    return 0;}
//Control ingreso de datos
int getch(void){
    struct termios oldattr, newattr;
    int ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
    }
// ESTE ES PARA INT!!!
void scandigit  (int length, int  * number){
    struct termios oldattr, newattr;
    int ch, counter = 0;
    char * array;
    array = malloc(length);

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    do{
        ch = getchar();

        if(ch == 127 && counter > 0)
        {
            printf("\b \b");
            counter--;
            array[counter] = '\0';
        }

        else if((counter < length) && (ch >= 48 && ch <= 57))
        {
            printf("%c", ch);
            array[counter] = ch;
            counter++;
        }
    }
    while(ch != 10 || array[0] == '\0');

    *number = atoi(array);
    free(array);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    }
// ESTE ES PARA LONG!!!
void scanlidigit(int length, long * number)
{
    struct termios oldattr, newattr;
    int ch, counter = 0;
    char * array;
    array = malloc(length);

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

    do
    {
        ch = getchar();

        if(ch == 127 && counter > 0)
        {
            printf("\b \b");
            counter--;
            array[counter] = '\0';
        }

        else if((counter < length) && (ch >= 48 && ch <= 57))
        {
            printf("%c", ch);
            array[counter] = ch;
            counter++;
        }
    }
    while(ch != 10 || array[0] == '\0');

    *number = strtol(array, NULL, 10);
    free(array);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);}
void scanletter (int length, char * array){
    struct termios oldattr, newattr;
    int ch, counter = 0;

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

    for(i = 0; array[i]; i++)
    { array[i] = '\0'; }

    do
    {
        ch = getchar();

        if(ch == 127 && counter > 0)
        {
            printf("\b \b");
            counter--;
            array[counter] = '\0';
        }

        else if( (counter < length) && ((ch == 32) ||
                (ch >= 65 && ch <= 90) ||
                (ch >= 97 && ch <= 122)))
        {
            printf("%c", ch);
            array[counter] = ch;
            counter++;
        }
    }
    while(ch != 10 || array[0] == '\0');

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);}                                   //Scan solo de letras
void scandecimal(int length, float * decimal){
    struct termios oldattr, newattr;
    int ch, separator, counter = 0, flag = 0;
    char * array;
    array = malloc(length);

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

    do
    {
        ch = getchar();

        if(ch == 127 && counter > 0)
        {
            printf("\b \b");
            counter--;
            array[counter] = '\0';

            if((counter == separator) && (flag == 1))
            { flag = 0; }
        }

        else if( (counter < length) && (ch >= 48 && ch <= 57) ||
                ((ch == 46) && (flag == 0)) )
        {
            printf("%c", ch);
            array[counter] = ch;

            if(ch == 46)
            {
                flag = 1;
                separator = counter;
            }

            counter++;
        }
    }
    while(ch != 10 || array[0] == '\0');

    *decimal = atof(array);
    free(array);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);}                                //Scan decimales
void scanchar   (int length, char * character, char * dictionary){
    struct termios oldattr, newattr;
    int ch, counter = 0;

    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

    for(i = 0; character[i]; i++)
    { character[i] = '\0'; }

    do
    {
        ch = getchar();

        if(ch == 127 && counter > 0)
        {
            printf("\b \b");
            counter--;
            *character = '\0';
        }

        else if(counter < length)
        {
            for(i = 0; i < strlen(dictionary); i++)
            {
                if(ch == dictionary[i])
                {
                    printf("%c", ch);
                    *character = ch;
                    counter++;
                }
            }

        }
    }
    while(ch != 10 || character[0] == '\0');

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);}            //Scan simbolos solo del argumento Diccionario
//Manejo de info (Opciones del menu)
void ingresar(void){
    system("clear");
    printf("\n\t--- Ingrese datos de la mascota ---");
    printf("\n\tNombre [32 max.]:\t");	scanletter  (32, dog->nombre);
    printf("\n\tEdad:\t\t\t");          scandigit   (3, &dog->edad);
    printf("\n\tRaza [16 max.]:\t\t");	scanletter  (16, dog->raza);
    printf("\n\tEstatura [cm]:\t\t");   scandigit   (3, &dog->estatura);
    printf("\n\tPeso [Kg]:\t\t");	    scandecimal (6, &dog->peso);
    printf("\n\tGénero [H/M]:\t\t");    scanchar    (1, &dog->genero, "HMhm");
    //Envio de dogType
    r = send(socket_cliente, dog, SIZE_DATA_DOG, 0);
    if(r == -1){
        perror("\n\t Error al enviar la estructura...");
        exit(EXIT_FAILURE);
    }
    //rec # actual de registro
    r = recv(socket_cliente, &positionReg, sizeof(positionReg),0);
    if(r == -1)
    {
        perror("\n\t Error al recibir #  de registro del perro...");
        exit(EXIT_FAILURE);
    }
     printf(
        "\n\n\t%s\n\t%s%li",
        "Datos registrados...",
        "No. actual de registro: ",
        positionReg
    );
    continuar(&menu);
    }
int  numreg(int a){

    //nregisters = (ftell(fp) - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8);
    printf("\n\tRegistros: %li", nregisters);
    if(nregisters > 0){
        printf("\n\tIngrese número de registro:\n\t");
        scanlidigit(10, &positionReg);
        if(positionReg < 1 || positionReg > nregisters){
            printf("\n\n\tFuera de rango [%li max].", nregisters);
            return 0;
        }else{
            if(a==0){
                return 1;
            }else{
                return 2;
            }
            
        }
    }else{
      printf("\n\tFallo en obtener número de registros.");
      return 0;
    }
    }
void ver(int modo){
  //Cod solicitar numero de registro y enviar a server
  //El numero de registro esta almacenado en positionReg
  if(modo==0||modo==2){//Otro
    modo = numreg(modo);
  }
  if(modo == 1){//Ver registro
    system("clear");
    printf("\n\n\t%s \n\t%s\t\t%s \n\t%s\t\t%i \n\t%s\t\t%s \n\t%s\t%i%s \n\t%s\t\t%.2f%s \n\t%s\t\t%c \n\t%s",
                    "------------ Mascota ------------",
                    "Nombre:"   , dog->nombre,
                    "Edad:"     , dog->edad,
                    "Raza:"     , dog->raza,
                    "Estatura:" , dog->estatura, " cm(s)",
                    "Peso:"     , dog->peso,     " Kg(s)",
                    "Genero:"   , dog->genero,
                    "---------------------------------"
    );
    char tonano[40]="gedit ";
    char numero[10]="";
    sprintf(numero, "%li", positionReg);
    strcat(tonano,numero);//Crea una linea de comando que se ejecutará por system()
    system(tonano);
  }else if(modo == 2){//Borrar registro
    system("clear");
    printf("\n\n\t%s \n\t%s\t\t%s \n\t%s\t\t%i \n\t%s\t\t%s \n\t%s\t%i%s \n\t%s\t\t%.2f%s \n\t%s\t\t%c \n\t%s",
                    "------------ Mascota ------------",
                    "Nombre:"   , dog->nombre,
                    "Edad:"     , dog->edad,
                    "Raza:"     , dog->raza,
                    "Estatura:" , dog->estatura, " cm(s)",
                    "Peso:"     , dog->peso,     " Kg(s)",
                    "Genero:"   , dog->genero,
                    "---------------------------------"
    );
  }
  continuar(&menu);
    }
void buscar(void){
    system("clear");
    char * dogName;
    dogName = malloc(32);
    dogName[32] = '\0';

    printf("\n\t--- Búsqueda ---");
    printf("\n\tNombre de la mascota: ");
    scanletter(32, dogName);

    if(j < 1){
        printf("\n\t%s%s%s\n", "No existe registro de la mascota \"", dogName, "\"...");
    }else{
        printf("%s\n",dogName);
        //Muetra todas las mascotas del mismo nombre
    }

    free(dogName);
    continuar(&menu);
    }
void borrar(void){
    ver(2);
    printf("\n\t%s%li%s\n\t", "¿Desea borrar el registro No. ", positionReg, "? [S/N]");
    scanchar(1, &opcion, "SYNsyn");
    if(opcion == 'n' || opcion == 'N'){
      continuar(&menu);
    }else{
      //Cod, pedir server borrado
      printf("\n\tRegistro borrado satisfactoriamente");
      continuar(&menu);
    }
    }
//Funciones de Interface
void menu(){
    printf("\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n\t",
        "1. Ingresar registro",
        "2. Ver registro",
        "3. Borrar registro",
        "4. Buscar registro",
        "5. Salir"
    );
    scanchar(1, &opcion, "12345");
    if(opcion == '5'){
        regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
    }else{
        system("clear");
        switch(opcion){
            case '1':
                opcion = '1';
                r = send(socket_cliente, &opcion,sizeof(opcion),0);        
                ingresar();
                break;

            case '2': 
                opcion = '1';
                r = send(socket_cliente,&opcion,sizeof(opcion),0); 
                ver(0);
                break;
            case '3':
                opcion = '1';
                r = send(socket_cliente,&opcion,sizeof(opcion),0);
                borrar();
                break;
            case '4':        
                opcion = '1';
                r = send(socket_cliente,&opcion,sizeof(opcion),0); 
                buscar();
                break;

        }
    }}
void continuar(void (*dst)(void)){
    printf("\n\n\tPresione cualquier tecla para continuar...");
    getch();
    system("clear");
    (*dst)();
    }
void regresar(void (*src)(void), void (*dst)(void), char * message){
    printf("\n\t%s\n\t", message);
    scanchar(1, &opcion, "SYNsyn");
    system("clear");

    if(opcion == 'n' || opcion == 'N'){
      (*src)();
    }else{
      (*dst)();
    }
    }
void salir(){
    free(dog);
    close(socket_cliente);
    exit(0);
    }
