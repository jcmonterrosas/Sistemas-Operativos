#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

FILE *fp;
char opcion;
int i = 0, j = 0;
long positionReg = 0, block = 0, nregisters = 0;

#define SIZE_HASH 2000
#define NAME_FILE "dataDogs.dat"


void ShowBar(int Percent,int MaxSize,char* Msg)
{
    int i = 0,a = 0;
        for(i = MaxSize+strlen(Msg)+2; i>=0; --i) putchar('\b'); // se va al principio de la barra

    putchar('[');
        for(i = (Percent*MaxSize)/10000000; a<i; ++a) putchar('#');
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

//----
int seeknode(long hashnumber);
void push(struct dogType *dog);

void ingresar(void);
void ver(int modo);
void borrar(void);
void buscar(void);

void regresar(void (*src)(void), void (*dst)(void), char * message);
void continuar(void (*dst)(void));
void menu(void);
//---


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

/*
*  Captura valor ingresado por usuario donde sólo
*  permite ingresar letras a-z, A-Z y espacio.
*/
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

/*
*  Captura valor ingresado por usuario donde sólo
*  permite ingresar los caracteres especificados
*  en el diccionario.
*/
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

long seekreg(long n, long hashnumber)
{
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

    return n;
}

void push(struct dogType *dog)
{
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
    }
}

int pop(char * parameter)
{
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
    }
}

/*int seeknode(long hashnumber)
{
    if(hashnumber < 0 || hashnumber > 2000)
    {
        printf("\n\tError en número hash: %li", hashnumber);
        return 0;
    }

    else
    {
        fseek(archivo, (hashnumber * 8) + 8, SEEK_SET);
        
        if(leer(&posnode, 8, 1) == 1)
        {
            if(posnode == -1)
            {
                printf("\n\tError en búsqueda de nodo.");
                return 0;
            }

            else
            {
                if(block == 0)
                { return searchnode(hashnumber, posnode); }
        
                else
                { return searchnode(block, posnode); }
            }
        }
    }
}*/

void truncar(long n)
{
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
                printf("\n\n\t(NU) - Error en busqueda de registro No. %li", n);
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
                printf("\n\n\t(NI) - Error en busqueda de registro No. %li", n);
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
}

int numreg()
{
    fseek(fp, 0, SEEK_END);
    nregisters = (ftell(fp) - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8);

    printf("\n\tRegistros: %li", nregisters);

    if(nregisters > 0)
    {
        printf("\n\tIngrese número de registro:\n\t");
        scanlidigit(10, &positionReg);

        if(positionReg < 1 || positionReg > nregisters)
        {
            printf("\n\n\tFuera de rango [%li max].", nregisters);
            return 0;
        }

        else
        { return 1; }
    }

    else
    {
        printf("\n\tFallo en obtener número de registros.");
        return 0;
    }
}

void ingresar(void)
{
    system("clear");
    printf("\n\t--- Ingrese datos de la mascota ---");
    printf("\n\tNombre [32 max.]:\t");		scanletter(32, dog->nombre);
    printf("\n\tEdad:\t\t\t");			scandigit(3, &dog->edad);
    printf("\n\tRaza [16 max.]:\t\t");		scanletter(16, dog->raza);
    printf("\n\tEstatura [cm]:\t\t");		scandigit(3, &dog->estatura);
    printf("\n\tPeso [Kg]:\t\t");			scandecimal(6, &dog->peso);
    printf("\n\tGénero [H/M]:\t\t");		scanchar(1, &dog->genero, "HMhm");
    push(dog);

    printf
    (
        "\n\n\t%s\n\t%s%li",
        "Datos registrados...",
        "No. actual de registro: ",
        ((positionReg - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8)) + 1
    );

    continuar(&menu);
}

void ver(int modo)
{
    if(numreg() == 1)
    {   
        block = (positionReg - 1) * (SIZE_DATA_DOG + 8) + (SIZE_HASH * 16);
        block = ((block - (SIZE_HASH * 16)) / (SIZE_DATA_DOG + 8)) + 1;
        fseek(fp, (positionReg - 1) * (SIZE_DATA_DOG + 8) + (SIZE_HASH * 16), SEEK_SET);

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

    if(modo == 0)
    { continuar(&menu); }

}

void borrar(void)
{
    ver(1);
    printf("\n\t%s%li%s\n\t", "¿Desea borrar el registro No. ", positionReg, "? [S/N]");
    scanchar(1, &opcion, "SYNsyn");

    if(opcion == 'n' || opcion == 'N')
    { continuar(&menu); }

    else
    {
        truncar(positionReg);
        printf("\n\tRegistro borrado satisfactoriamente");
        continuar(&menu);
    }
}

void buscar(void){
    system("clear");
    char * dogName;
    dogName = malloc(32);
    dogName[32] = '\0';

    printf("\n\t--- Búsqueda ---");
    printf("\n\tNombre de la mascota: ");
    scanletter(32, dogName);
    pop(dogName);
    if(j < 1){
        printf("\n\t%s%s%s\n", "No existe registro de la mascota \"", dogName, "\"...");
    }

    free(dogName);
    continuar(&menu);
}

void formatear()
{
    block = 0;
    j = 2 * SIZE_HASH;

    for(i = 0; i < j; i++)
    {
        fwrite(&block, 8, 1, fp);
    }

    printf("\n\n\tProceso de formato finalizado.");
}

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
            case '1':        ingresar();	break;
            case '2':        ver(0);		break;
            case '3':        borrar();		break;
            case '4':        buscar();		break;
            default:
            regresar(&menu, &salir, "¿Desea salir del programa? [S/N]:");
            }
    }
}

void main(){
    system("clear");												//Elimina todos los elementos de la terminal
    fp = fopen(NAME_FILE, "rb+");										//Abre el archivo (dataDogs.dat)
    																//NOTA: El 'b' despues del argumento de
    																//	lectura/escritura del archivo indica se se
    																//	manejará de manera binaria
    																//		w crea uno nuevo, aunque ya exista.
    																//		r lo abre, si no existe, lo crea
    																//		a lo abre solo para editar final de archivo
    																//		+ permite lectura y escritura
    																//	Fuente: https://www.tutorialesprogramacionya.com/cya/detalleconcepto.php?punto=71&codigo=71&inicio=60
    dog = malloc(SIZE_DATA_DOG);										//Var dog de tipo dogType, espacio de memoria que
    																//	contiene un registro. Acá solo se inicializa
    fflush(fp);													//Limpia el buffer de fp
    																//IF para verificar existencia del archivo
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
            formatear();												//?? (REinicia algunas variables, sin funcionalidad conocida)
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
                        ShowBar(k,100,"  Procesando dataDogs.dat");
                        //sleep(1);
                    }
                    free(dogName);
                }

                fclose(petNames);
            }

            continuar(&menu);
        }
    }else{
        system("clear");
    }
    menu();
}
