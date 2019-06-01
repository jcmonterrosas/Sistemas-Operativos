#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

#define TECLA_ARRIBA 72
#define TECLA_ABAJO 80
#define ENTER 13

void menu_principal();
void menu_objetos();
void menu_espacios();
void menu_listas();
int menu(const char *titulo, const char *opciones[], int n);

/*void gotoxy(int x, int y){
    HANDLE hCon;
    hCon = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hCon,dwPos);
}*/

/*int ** crear_matrix(int n, int m){
    int ** temp = new int*[n];
    for(int i=0; i<n; i++){
        temp[i]=new int[m];
    }
    return temp;
}
char ** leer_nombres(istream& ifs,int n,int m){
    char ** nombres = new char*[n];
    for(int i=0; i<n; i++){
        nombres[i]=new char[m];
        ifs.getline(nombres[i],m);
    }
    return nombres;
}
ostream& escribir_int(ostream& os, int a){
os << a;
return os;
};
ostream& escribir_char(ostream& os, char a){
os << a;
return os;
};
ostream& escribir_matriz_int(ostream& os, int** X, int n, int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            escribir_int(os, X[i][j]);
            escribir_char(os, '\t');
        }
        escribir_char(os, '\n');
    }
    return os;
}
void imprimir_nombres(char ** x, int n){
    for(int i=0; i<n; i++){
        cout<<x[i]<<endl;
    }
}
char nombre_espacio(){
    char nombre_espacio[50];
    cout << "\n\tNombre del nuevo espacio:   ";
    gets(nombre_espacio);
    ofstream entrada(nombre_espacio);
    cout << "\n\tNuevo Espacio <"<<nombre_espacio<<"> disponible para su uso.";
}
*/
int main()
{
   menu_principal();

    return 0;
}

void menu_principal()
{
   bool repite = true;
   int opcion;

   // Título y las opciones del menú
   const char *titulo = "MENU PRINCIPAL";
   const char *opciones[] = {"Administrar Objetos", "Administrar Espacios", "Listas", "Salir"};
   int n = 4; // Número de opciones

   do {
      opcion = menu(titulo, opciones, n);

      // Alternativas
      switch (opcion) {

         case 1:
            menu_objetos();
            break;

         case 2:
            menu_espacios();
            break;

         case 3:
            menu_listas();
            break;

         case 4:
            repite = false;
            break;
      }

   } while (repite);
}

void menu_objetos()
{
   bool repite = true;
   int opcion;
   char nombre;

   const char *titulo = "MENU OBJETOS";
   const char *opciones[] = {"Buscar Objeto", "Guardar Objeto", "Borrar objeto", "Regresar"};
   int n = 4;

   do {
      opcion = menu(titulo, opciones, n);

      // Alternativas
      system("cls");
      switch (opcion) {

         case 1:
            system("pause>nul");
            break;

         case 2:

            system("pause>nul");
            break;

         case 3:

            system("pause>nul");
            break;

         case 4:
            repite = false;
            break;
      }

   } while (repite);
}

void menu_espacios()
{
   bool repite = true;
   int opcion;


   const char *titulo = "MENU ESPACIOS";
   const char *opciones[] = {"Crear Espacio", "Eliminar Espacio", "Regresar"};
   int n = 3;

   do {
      opcion = menu(titulo, opciones, n);

      // Alternativas
      system("cls");
      switch (opcion) {

         case 1:
            nombre_espacio();
            system("pause>nul");
            break;

         case 2:

            system("pause>nul");
            break;

         case 3:
            repite = false;
            break;
      }

   } while (repite);
}

void menu_listas()
{
   bool repite = true;
   int opcion;

   const char *titulo = "MENU MULTIPLICACION";
   const char *opciones[] = {"Ver Lista de Objetos", "Ver Lista de Espacios", "Regresar"};
   int n = 3;

   do {
      opcion = menu(titulo, opciones, n);

      // Alternativas
      system("cls");
      switch (opcion) {

         case 1:
            system("pause>nul");
            break;

         case 2:

            system("pause>nul");
            break;

         case 3:
            repite = false;
            break;
      }

   } while (repite);
}
int menu(const char *titulo, const char *opciones[], int n)
{
   int opcionSeleccionada = 1;
   int tecla;
   bool repite = true;

   do {
      system("cls");

      gotoxy(5, 3 + opcionSeleccionada);
      printf("==>");

      // Imprime el título
      gotoxy(15, 2);
      printf("%s", titulo);

      // Imprime las opciones
      for (int i = 0; i < n; i++) {
         gotoxy(10, 4 + i); 
         printf("%i ) %s",(i+1),opciones[i]);
      }

      do {
         tecla = getch();
      } while (tecla != TECLA_ARRIBA && tecla != TECLA_ABAJO && tecla != ENTER);

      switch (tecla) {
         case TECLA_ARRIBA:
            opcionSeleccionada--;

            if (opcionSeleccionada < 1) {
               opcionSeleccionada = n;
            }
            break;

         case TECLA_ABAJO:
            opcionSeleccionada++;

            if (opcionSeleccionada > n) {
               opcionSeleccionada = 1;
            }
            break;

         case ENTER:
            repite = false;
            break;
      }

   } while (repite);
   return opcionSeleccionada;
}