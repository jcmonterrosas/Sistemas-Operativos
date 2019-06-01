	
	#include <termios.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/types.h>

	FILE *archivo;
	FILE *dogNames;
	
	char opcion, file[] = "dataDogs.dat";
	char * nombre;
	int i, j;
	long posreg, posnode = 0, posdir = 0, block = 0, nregisters = 0;
	time_t t;
    
	struct dogType
	{
		char nombre[32];
		int edad;
		char raza[16];
		int estatura;
		float peso;
		char genero;
	};

	struct dogType *pet;

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

	void menu(void);
	int seeknode(long hashnumber);
	int searchnode(long hashnumber, long dircluster);
	int push(struct dogType *pet);
	void pop(char * parameter);

	void formatear();
	void ampliar();


	int getch(void)
	{
		struct termios oldattr, newattr;
		int ch;
		tcgetattr(STDIN_FILENO, &oldattr);
		newattr = oldattr;
		newattr.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
		ch = getchar();
		ch = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
		return ch;
	}

	long hash(const char *str)
	{
		long hashnumber;
		const char *ustr;

		ustr = (const char *) str;
	 
		hashnumber = 0;

		while(*ustr != '\0')
		{
			hashnumber = (hashnumber * 256 + *ustr) % 2000;
			ustr++;
		}

		if(hashnumber < 0)
		{
			hashnumber *= -1;
		}

		return hashnumber;
	}

	int push(struct dogType *pet)
	{
		if(abrir(file, "ab+") == 1)
		{
			escribir(pet, sizeof(struct dogType), 1);
			// Ubicación del registro
			posreg = ftell(archivo) - sizeof(struct dogType);

			if(seeknode(hash(pet -> nombre)) == 1)
			{
				do
				{
					leer(&block, 8, 1);

					if(block == -1)
					{
						// control de colisión
						printf("\nColision\n");
						exit(0);
						break;
					}
				}
				while(block != 0);

				posdir = ftell(archivo) - 8;
				fclose(archivo);

				if(abrir(file, "rb+") == 1)
				{
					fseek(archivo, posdir, SEEK_SET);
					
					if(escribir(&posreg, 8, 1) == 0)
					{
						printf("\n\tError en ingresar dirección de registro.");
						return 0;
					}

					fclose(archivo);
				}

			}
		}

		return 1;
	}

	void pop(char * parameter)
	{
		if(abrir(file, "rb+") == 1)
		{
			if(seeknode(hash(parameter)) == 1)
			{
				char * name = malloc(32);
				posnode = ftell(archivo) - 8;

				while(1)
				{
					posnode += 8;
					fseek(archivo, posnode, SEEK_SET);
					leer(&posreg, 8, 1);

					if(posreg == -1)
					{ break; }

					if(posreg == 0)
					{ continue; }

					fseek(archivo, posreg, SEEK_SET);
					leer(name, sizeof(struct dogType), 1);

					if(strcmp(parameter, name) == 0)
					{
						j++;
						posreg = (posreg - 52847952) / 64;
						printf
						(
							"\n\n\t%s%li \t\t%s%s \n",
							"No. ", posreg,"Nombre: ", pet->nombre
						);
					}
				}

				free(name);
				fclose(archivo);
			}
		}
	}

	int seeknode(long hashnumber)
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
	}

	int searchnode(long hashnumber, long dircluster)
	{
		long cluster;

		fseek(archivo, dircluster, SEEK_SET);
		leer(&cluster, 8, 1);

		// Verificación de nodo
		if(cluster < -1 && -hashnumber-2 == cluster)
		{
			return 1;
		}

		// Fin del bloque de posiciones de memoria
		else if(cluster == -1)
		{
			j = 0;
			searchnode(hashnumber, dircluster + 8);
		}

		else if(cluster > -1)
		{
			if(j == 0)
			{ block = hashnumber; }

			j++;
			seeknode(j - 1);
		}
		
		else
		{
			printf("\n\tError en lectura de nodo.");
			return 0;
		}
	}

	int abrir(char * fuente, char * modo)
	{
		archivo = fopen(fuente, modo);
	
		if(archivo == NULL)
		{
			printf
			(
				"\n\t%s%s%s\n\t%s\n\t",
				"El archivo \"", fuente, "\" no fue encontrado...",
				"¿Desea crearlo? [S/N]:"
			);

			scanf(" %s", &opcion);

			if(opcion == 'N' || opcion == 'n')
			{ return 0; }

			else
			{
				if(abrir(fuente, "w") == 1)
				{
					fclose(archivo);

					if(abrir(fuente, modo) == 1)
					{
						printf("\n\n\t%s%s%s", "Generando \"", file, "\"...");
						return 2;
					}

					else
					{
						printf("\n\n\t%s%s%s", "Error generando archivo\"", file, "\".");
						return 0;
					}
				}

				else
				{ return 0; }
			}
		}

		return 1;
	}

	int leer(void * registro, size_t tam, size_t nregistros)
	{
		if(fread(registro, tam, nregistros, archivo) <= 0)
		{
			printf("\n\tError de lectura...");
			return 0;
		}

		fflush(archivo);
		return 1;
	}

	int escribir(void * registro, size_t tam, size_t nregistros)
	{
		if(fwrite(registro, tam, nregistros, archivo) <= 0)
		{
			printf("\n\tError de escritura...");
			return 0;
		}

		fflush(archivo);
		return 1;
	}

	void regresar(void (*src)(void), void (*dst)(void), char * message)
	{
		printf("\n\t%s\n\t", message);
		scanf(" %s", &opcion);
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

	int numreg()
	{
		rewind(archivo);

		if(leer(&nregisters, 8, 1) == 1)
		{
			printf("\n\tRegistros: %li", nregisters);

			if(nregisters != 0)		
			{
				printf("\n\tIngrese número de registro:\n\t");
				scanf(" %li", &posreg);

				posreg = (64 * posreg) + 52847952;
				return 1;
			}

			else
			{ return 2; }
		}

		else
		{
			printf("\n\tFallo en obtener número de registros.");
			return 0;
		}

		return 1;
	}

	void ampliar()
	{
		long cola = ftell(archivo) - 8;

		fseek(archivo, 0L, SEEK_END);
		long posregfin = ftell(archivo) - sizeof(struct dogType);
		long tam = ftell(archivo) + (2200 * 8);


		// Posicion de la cola del ultimo nodo
		fseek(archivo, 16000, SEEK_SET);
		leer(&block, 8, 1);

		fseek(archivo, block + 2200, SEEK_SET);

		do
		{
			leer(&block, 8, 1);
		}
		while(block != -1);

		long lennodes = ftell(archivo) - 8;


		if(ftruncate(fileno(archivo), tam) == 0)
		{
			tam = (2200 * 8) - sizeof(struct dogType);

			do
			{
				fseek(archivo, posregfin, SEEK_SET);
				leer(pet, sizeof(struct dogType), 1);

				fseek(archivo, tam, SEEK_CUR);
				posreg = ftell(archivo);
				escribir(pet, sizeof(struct dogType), 1);

				if(seeknode(hash(pet->nombre)) == 1)
				{
					do
					{
						leer(&block, 8, 1);

						if(block == posregfin)
						{
							fseek(archivo, -8, SEEK_CUR);
							escribir(&posreg, 8, 1);
							break;
						}
					}
					while(block != -1);
				}

				posregfin -= sizeof(struct dogType);
			}
			while(posregfin >= (tam + lennodes));

			do
			{
				fseek(archivo, lennodes, SEEK_CUR);
				leer(&block, 8, 1);
				escribir(&block, 8, 1);
				lennodes -= 8;
			}
			while(lennodes >= cola + (2200 * 8));
		}

		block = 0;

		fseek(archivo, cola, SEEK_SET);

		for(i = 0; i < 2200; i++)
		{
			escribir(&block, 8, 1);
		}

		block = -1;
		escribir(&block, 8, 1);

		// Actualización de posiciones de registros dezplazados - Nota: Mejorar código
		do
		{
			fseek(archivo, cola - 2200, SEEK_SET);
			leer(&block, 8, 1);
			cola -= 8;
		}
		while(block >= -1);

		posreg = ftell(archivo) - 8;
		fseek(archivo, 8, SEEK_SET);

		do
		{
			leer(&block, 8, 1);
			
			if(block == posreg)
			{
				do
				{
					leer(&block, 8, 1);

					if(block == -1)
					{ break; }

					block += 2200;
					fseek(archivo, - 8, SEEK_CUR);
					escribir(&block, 8, 1);
				}
				while(block != -1);

				break;
			}
		}
		while(block != -1);
	}

	void truncar(long pos)
	{
		if(abrir(file, "rb+") == 1)
		{
			fseek(archivo, 0, SEEK_END);
			long eof = ftell(archivo) - sizeof(struct dogType) - 1;
			long posregnext;

			fseek(archivo, pos, SEEK_SET);
			block = 0;
			escribir(&block, 8, 1);

			if(pos < eof)
			{
				do
				{
					posregnext = pos + sizeof(struct dogType);
					fseek(archivo, posregnext, SEEK_SET);
					leer(pet, sizeof(struct dogType), 1);

					if(seeknode(hash(pet->nombre)) == 1)
					{
						escribir(&pos, 8, 1);
						fseek(archivo, pos, SEEK_SET);
						escribir(pet, sizeof(struct dogType), 1);
					}

					pos = posregnext;
				}
				while(ftell(archivo) <= eof);
			}

			if(ftruncate(fileno(archivo), eof + 1) == 0)
			{
				rewind(archivo);
				leer(&block, 8, 1);
				block -= 1;

				rewind(archivo);
				escribir(&block, 8, 1	);

				printf("\n\tRegistro borrado");
			}

			else
			{
				printf("\n\tError en borrar registro...");
			}

			fclose(archivo);	
		}
	}

	void ingresar(void)
	{
		if(abrir(file, "rb+") == 1)
		{
			system("clear");
			rewind(archivo);

			if(leer(&nregisters, 8, 1) == 1)
			{
				printf("\n\t--- Ingrese datos de la mascota ---");
				printf("\n\tNombre [32 max.]:\t");		scanf(" %s", pet->nombre);
				printf("\n\tEdad:\t\t\t");				scanf(" %i", &pet->edad);
				printf("\n\tRaza [16 max.]:\t\t");		scanf(" %s", pet->raza);
				printf("\n\tEstatura [cm]:\t\t");		scanf(" %i", &pet->estatura);
				printf("\n\tPeso [Kg]:\t\t");			scanf(" %f", &pet->peso);
				printf("\n\tGénero [H/M]:\t\t");		scanf(" %s", &pet->genero);

				fclose(archivo);

				if(push(pet) == 1)
				{
					if(abrir(file, "rb+") == 1)
					{
						rewind(archivo);
						nregisters++;

						if(escribir(&nregisters, 8, 1) == 1)
						{
							posreg = (posreg - 52847952) / 64;
							printf
							(
								"\n\n\t%s\n\t%s%li.",
								"Datos registrados satisfactoriamente",
								"No. actual de registro asignado: ",
								posreg
							);
						}

						fclose(archivo);
					}
				
				}
			}

			else
			{
				printf("\n\tFallo en obtener número de registros.");
			}
		}

		continuar(&menu);
	}

	void ver(int modo)
	{
		if(abrir(file, "rb+") == 1)
		{
			if(numreg() == 1)
			{
				fseek(archivo, posreg, SEEK_SET);
				if(leer(pet, sizeof(struct dogType), 1) == 1)
				{
					printf(
						"\n\n\t%s \n\t%s\t\t%s \n\t%s\t\t%i \n\t%s\t\t%s \n\t%s\t%i%s \n\t%s\t\t%.2f%s \n\t%s\t\t%c \n\t%s",
						"||||||||||| Mascota ||||||||||||",
						"Nombre:", pet->nombre,
						"Edad:", pet->edad,
						"Raza:", pet->raza, 
						"Estatura:", pet->estatura, " cm(s)",
						"Peso:", pet->peso, " Kg(s)",
						"Genero:", pet->genero,
						"|||||||||||||||||||||||||||||||||"
					);
				}
			}

			fclose(archivo);
		}
		
		if(modo == 0)
		{ continuar(&menu); }
	}

	void borrar(void)
	{
		ver(1);
		long pos = posreg;
		posreg = (posreg - 52847952) / 64;
		printf("\n\t%s%li%s\n\t", "¿Quiere borrar el registro No. ", posreg, "? (s/n)");
		scanf(" %s", &opcion);

		if(opcion == 'n' || opcion == 'N')
		{ menu(); }
		
		else
		{ truncar(pos); }

		continuar(&menu);
	}

	void buscar(void)
	{
		system("clear");
		printf("\n\t||| Búsqueda |||");
		printf("\n\tNombre de la mascota: ");
		scanf(" %s", nombre);

		pop(nombre);

		if(j < 1)
		{
			printf("\n\t%s%s%s\n", "No existe registro de la mascota \"", nombre, "\"...");
		}

		continuar(&menu);
	}

	void salir()
	{
		free(nombre);
		free(pet);
		fclose(archivo);
		exit(0);
	}

	void menu()
	{
		printf
		(
			"\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\n\t",
			"|||||| Bienvenido ||||||",
			"1. Ingresar",
			"2. Ver",
			"3. Borrar",
			"4. Buscar",
			"5. Salir"
		);

		scanf(" %s", &opcion);

		if(opcion == '5')
		{
			regresar(&menu, &salir, "¿Quiere salir del programa? (s/n):");
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
				regresar(&menu, &salir, "¿Quiere salir del programa? (s/n):");
		    	}
		}
	}

	void formatear()
	{
		/* Bloque de números de registros activos */
		block = 0;
		escribir(&block, 8, 1);

		/* Bloque de direcciones de cada nodo */
		for(i = 0; i < 2000; i++)
		{
			block = (26416 * i) + 16016;
			escribir(&block, 8, 1);
		}

		block = -1;
		escribir(&block, 8, 1);

		/* Bloques de almacenamiento para cada nodo del arreglo */
		for(i = 2; i < 2002; i++)
		{	
			block = -i;
			escribir(&block, 8, 1);

			block = 0;

			for(j = 0; j < 3300; j++)
			{
				escribir(&block, 8, 1);
			}

			block = -1;
			escribir(&block, 8, 1);
		}

		printf("\n\tProceso de formato finalizado.");
		block = 0;
	}

	void main(void)
	{
		nombre = malloc(32);
		pet = malloc(sizeof(struct dogType));
		int state_file = abrir(file, "rb+");

		if(state_file == 0)
		{ exit(0); }

		else if(state_file == 2)
		{
			printf("\n\tArchivo creado satisfactoriamente.");
			formatear();
			fclose(archivo);

			dogNames = fopen("nombresMascotas.dat" , "rb+");
			srand((unsigned) time(&t));

			for(i = 0; i < 250000; i++)
			{
				fseek(dogNames, ((rand() % 1716) * 32), SEEK_SET);
				fgets(nombre, 32, dogNames);
				strcpy(pet->nombre, nombre);
				//printf("%s, ", pet->nombre);
				pet->edad = (rand() % 19) + 1;
				strcpy(pet->raza, razas[rand() % 44]);
				pet->estatura = (rand() % 19) + 1;
				pet->peso = (rand() % 123) + 1;
				pet->genero = genero[rand() % 3];

				push(pet);
			}

			abrir(file, "rb+");
			fseek(archivo, 0, SEEK_SET);
			block = 250000;
			escribir(&block, 8, 1);

			fclose(dogNames);
			fclose(archivo);
			continuar(&menu);
		}

		else
		{
			fclose(archivo);
			system("clear");
		}
		
		menu();
	}
