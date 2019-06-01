#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //barra de carga
#include <unistd.h> //barra de carga
#include <time.h>

#define SIZE_HASH 2000
#define NAME_FILE "borrame.dat"

FILE *fp;
char opcion;
int i = 0;
long positionReg = 0;

int getch(void)
{
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

void scanletter(int length, char * array)
{
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

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

/*
*  Captura valor ingresado por usuario donde sólo
*  permite ingresar dígitos 0-9.
*/
void scandigit(int length, int * number)
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

    *number = atoi(array);
    free(array);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

/*
*  Captura valor ingresado por usuario donde sólo
*  permite ingresar dígitos 0-9.
*/
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
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

/*
*  Captura valor ingresado por usuario donde sólo
*  permite ingresar valores decimales 0-9 y separador decimal.
*/
void scandecimal(int length, float * decimal)
{
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
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

void scanchar(int length, char * character, char * dictionary)
{
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

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

struct node{
    int key;
    int val;
    struct node *next;
};
struct table{
    int size;
    struct node **list;
};
struct table *createTable(int size){
    struct table *t = (struct table*)malloc(sizeof(struct table));
    t->size = size;
    t->list = (struct node**)malloc(sizeof(struct node*)*size);
    int i;
    for(i=0;i<size;i++)
        t->list[i] = NULL;
    return t;
}
int hashCode(struct table *t,int key){
    if(key<0)
        return -(key%t->size);
    return key%t->size;
}
void insert(struct table *t,int key,int val){
    int pos = hashCode(t,key);
    struct node *list = t->list[pos];
    struct node *newNode = (struct node*)malloc(sizeof(struct node));
    struct node *temp = list;
    while(temp){
        if(temp->key==key){
            temp->val = val;
            return;
        }
        temp = temp->next;
    }
    newNode->key = key;
    newNode->val = val;
    newNode->next = list;
    t->list[pos] = newNode;
}

int lookup(struct table *t,int key){
    int pos = hashCode(t,key);
    struct node *list = t->list[pos];
    struct node *temp = list;
    while(temp){
        if(temp->key==key){
            return temp->val;
        }
        temp = temp->next;
    }
    return -1;
}

void ShowBar(int Percent,int MaxSize,char* Msg)
{
    int i = 0,a = 0;
        for(i = MaxSize+strlen(Msg)+2; i>=0; --i) putchar('\b'); // se va al principio de la barra

    putchar('[');
        for(i = (Percent*MaxSize)/100000; a<i; ++a) putchar('#');
        for(; a < MaxSize; ++a) putchar('-');
    putchar(']');

    printf(" %s",Msg);
    fflush(stdout);
}

struct dogType
{
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

int seeknode(long hashnumber);
void push(struct dogType *dog);

void ingresar(void);
void ver(int modo);
void borrar(void);
void buscar(void);

void regresar(void (*src)(void), void (*dst)(void), char * message);
void continuar(void (*dst)(void));
void menu(void);

long hash(const char *str)
{
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

    return hashnumber;
}

int leer(void * data, size_t size)
{
    if(fread(data, size, 1, fp) <= 0)
    {
        printf("\n\tError de lectura...");
        return 0;
    }

    fflush(fp);
    return 1;
}

int escribir(void * data, size_t size)
{
    if(fwrite(data, size, 1, fp) <= 0)
    {
        printf("\n\tError de escritura...");
        return 0;
    }

    fflush(fp);
    return 1;
}

/*void ver(int pos)
{
    if(numreg() == 1)
    {
        fseek(fp, pos, SEEK_SET);

        if(leer(dog, SIZE_DATA_DOG) == 1)
        {
            printf
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
            );

            //leer(&block, 8);
            //printf("\n\tposnext: %li\n", block);
        }
    }
}
*/
void regresar(void (*src)(void), void (*dst)(void), char * message)
{
    printf("\n\t%s\n\t", message);
    scanchar(1, &opcion, "SYNsyn");
    system("clear");

    if(opcion == 'n' || opcion == 'N')
    { (*src)(); }

    else
    { (*dst)(); }
}

void continuar(void (*dst)(void))
{
    printf("\n\n\tPresione cualquier tecla para continuar...");
    getch();
    system("clear");
    (*dst)();
}

void salir()
{
    fclose(fp);
    free(dog);
    exit(0);
}

void menu()
{
    printf
    (
        "\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n\t",
        "1. Ingresar registro",
        "2. Ver registro",
        "3. Borrar registro",
        "4. Buscar registro",
        "5. Salir"
    );

    scanchar(1, &opcion, "12345");

    if(opcion == '5')
    {
        regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
    }

    else
    {
        system("clear");
        switch(opcion)
            {
            case '1':        /*ingresar();*/	break;
            case '2':        /*ver(0);*/		  break;
            case '3':        /*borrar();*/		break;
            case '4':        /*buscar();*/		break;
            default:
            regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
            }
    }
}

int main(){
    struct table *t = createTable(100000);
    system("clear");												//Elimina todos los elementos de la terminal
    fp = fopen(NAME_FILE, "rb+");
    dog = malloc(SIZE_DATA_DOG);
    fflush(fp);
    if(fp == NULL){													//Si no existe:
        printf
        ("\n\t%s%s%s\n\t%s\n\t",										//Mensaje para confirmar creacion de un nuevo archv.
         "El archivo \"", NAME_FILE, "\" no fue encontrado...",
         "¿Desea crearlo? [S/N]:"
        );

        scanchar(1, &opcion, "SYNsyn");									//Captura respuesta del usuario

        if(opcion == 'N' || opcion == 'n'){								//En caso negativo, sale del programa.
        	exit(-1);
        }else{														//Caso contrario, lo crea
            fp = fopen(NAME_FILE, "w");									//Creacion del arch (con 'w')
            freopen(NAME_FILE, "rb+", fp);								//Abre el archivo y lo asigna a el flo de datos fp
            printf("\n\t%s\n\t",
                "¿Desea generar información aleatoria? [S/N]:");				//Creacion de informacion aleatoria.
                													//REQUISITO: Generado por un programa
                													//	(Entiendase: Proceso) independiente segun la
                													//	opcion generada llamara una funcion que creara
                													//	dicho proceso y creará los registros aleatorios
            scanchar(1, &opcion, "SYNsyn");								//Captura la opcion seleccionada
            printf("\n");
            //Desde aca

            if(opcion == 'Y' || opcion == 'y' || opcion == 'S' || opcion == 's'){
            														//Opcion afirmativa
                FILE *petNames;										//Declara el archivo petNames que contiene una lista
                petNames = fopen("nombresMascotas.dat" , "rb+");				//de nombres de perros para la creacion aleatoria de
                													//los registros

                if(petNames == NULL){
                    printf("\n\n\tEl archivo\"%s\"no existe.","nombresMascotas.dat");
                }else{
                    int k;
                    time_t tiempo;
                    srand((unsigned) time(&tiempo));
                    char * dogName;
                    dogName = malloc(32);
                    for(k = 0; k < 100000; k++){
                        fseek(petNames, ((rand() % 1716) * 32), SEEK_SET);
                        fgets(dogName, 32, petNames);
                        strcpy(dog->nombre, dogName);
                        dog->edad = (rand() % 19) + 1;
                        strcpy(dog->raza, razas[rand() % 44]);
                        dog->estatura = (rand() % 19) + 1;
                        dog->peso = (rand() % 123) + 1;
                        dog->genero = genero[rand() % 3];
                        positionReg = ftell(fp);
                        fwrite(dog, SIZE_DATA_DOG, 1, fp);
                        insert(t,hash(dog->nombre),positionReg);;
                        ShowBar(k,100,"  Procesando dataDogs.dat");
                        //sleep(1);
                    }
                    free(dogName);
                }

                fclose(petNames);
            }

        }
    }else{
        system("clear");
        printf("%d \n",lookup(t,hash("Abel")));
    }
    //ver(lookup(t,hash("Abel")));    
    return 0;
}   