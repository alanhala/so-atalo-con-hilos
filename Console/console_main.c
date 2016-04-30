/*
 * console_main.c
 *
 *  Created on: 28/4/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
//#include <commons/log.h>

#define KERNELTIP = "localhost"
#define KERNELPORT = "30000"

//Agrega Newton -- Inicio
void cargaArray(char array[], FILE* codeF);
void imprimeArray(const int cantCar,char array[]);
//Agrega Newton -- Fin
void connect_to_kernel();

int main(int argc, char **argv) {

//Agrega Newton -- Inicio

		//t_log *errorLogger;
		//errorLogger = log_create("LogErroresComandos.txt","levantaArchivoEnArray",true,LOG_LEVEL_ERROR);

		int cantCar, c; 	//cantCar: cantidad de caracteres en el archivo
							//c: cada caracter del archivo

		cantCar = 1;		//Se inicializa en 1 porque se debe agregar 1 caracter
							//a lo que se lea del archivo para el caracter de terminacion

		//Se abre el archivo de texto
		FILE *codeF = fopen("/home/utnso/workspace/LevantaArchivoEnArray/Debug/ansisopCode", "r");

		if(codeF==NULL){
			//log_error(errorLogger,"No se encuentra el codigo ansisop\n");
			return EXIT_FAILURE;
		}

		//Se obtiene la cantidad de filas y columnas del archivo de texto
		while ((c = getc(codeF)) != EOF)
		{
			cantCar++;
		}

		char array[cantCar]; //Declaracion del array donde se almacena el codigo

		rewind(codeF); //Coloca codeF al inicio del archivo

		//Se recorre el archivo y se va almacenando en el array
		if (codeF)
		{
			cargaArray(array, codeF);
		} else {
			//log_error(errorLogger,"No se puede cargar el codigo ansisop en el array\n");
			return EXIT_FAILURE;
		}

		fclose(codeF); //Cierra archivo

		imprimeArray(cantCar, array);

//Agrega Newton -- Fin

	connect_to_kernel();
	return 0;
}

void connect_to_kernel(){
	create_client_socket_descriptor("localhost", "8001");

}

//Agrega Newton -- Inicio
void cargaArray(char array[], FILE* codeF) {
	int c;
	int i = 0;
	for (i = 0; EOF; i++) {
		c = getc(codeF);
		if (c == EOF) {
			array[i] = '\0';
			break;
		} else {
			array[i] = c;
		}
	}
}
void imprimeArray(const int cantCar,char array[]) {
	int i = 0;
	for (i = 0; i < cantCar; i++)
		printf("%c", array[i]);
}
//Agrega Newton -- Fin
