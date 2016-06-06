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
#include "protocoloConsola.h"

//#include <commons/log.h>

#define KERNELTIP = "localhost"
#define KERNELPORT = "30000"

//Agrega Newton -- Inicio
//void cargaArray(char array[],  FILE* codeF);
//void imprimeArray(const int cantCar,char array[]);
//Agrega Newton -- Fin
int connect_to_kernel();

int main(int argc, char **argv) {

/*
//Agrega Newton -- Inicio

		//t_log *errorLogger;
		//errorLogger = log_create("LogErroresComandos.txt","levantaArchivoEnArray",true,LOG_LEVEL_ERROR);

		int cantCar, c; 	//cantCar: cantidad de caracteres en el archivo
							//c: cada caracter del archivo

		cantCar = 1;		//Se inicializa en 1 porque se debe agregar 1 caracter
							//a lo que se lea del archivo para el caracter de terminacion

		//Se abre el archivo de texto
		struct FILE *codeF = fopen("/home/utnso/workspace/LevantaArchivoEnArray/Debug/ansisopCode", "r");

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
*/
	int sockfd = connect_to_kernel();

	t_iniciar_programa_en_kernel *iniciar_programa = malloc(sizeof(t_iniciar_programa_en_kernel));
	memset(iniciar_programa,0,sizeof(t_iniciar_programa_en_kernel));

	iniciar_programa->codigo_de_programa = malloc(sizeof("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
	char saludo[511]="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	memcpy(iniciar_programa->codigo_de_programa,saludo,sizeof("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));

	t_stream *buffer = malloc(sizeof(t_stream));

	buffer = serializar_mensaje(91,iniciar_programa);

	send(sockfd,buffer->datos,buffer->size,0);

	t_header *un_header = malloc(sizeof(t_header));

	char buffer_header[5];

	int	bytes_recibidos_header,
		bytes_recibidos;

	bytes_recibidos_header = recv(sockfd,buffer_header,5,MSG_PEEK);

	un_header = deserializar_header(buffer_header);

	char buffer_recibidos[un_header->length];

	bytes_recibidos = recv(sockfd,buffer_recibidos,un_header->length,0);

	t_respuesta_iniciar_programa_en_kernel *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_kernel));

	respuesta = deserealizar_mensaje(92,buffer_recibidos);

	printf("Consola. La respuesta es: %d\n",respuesta->respuesta_correcta);

	return 0;
}

int connect_to_kernel(){
	int sockfd = create_client_socket_descriptor(NULL, "22000");
	return sockfd;
}

/*
//Agrega Newton -- Inicio
void cargaArray(char* array, struct FILE* codeF) {
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
*/
