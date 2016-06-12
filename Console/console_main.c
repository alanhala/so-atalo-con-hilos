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
#define KERNELPORT = "9000"

//Agrega Newton -- Inicio
//void cargaArray(char array[],  FILE* codeF);
//void imprimeArray(const int cantCar,char array[]);
//Agrega Newton -- Fin

int main(int argc, char **argv) {

	int kernel_socket_descriptor = create_client_socket_descriptor("localhost", "9000");
	int a =2;
	send(kernel_socket_descriptor, &a, sizeof(int), 0);

	t_iniciar_programa_en_kernel *iniciar_programa = malloc(sizeof(t_iniciar_programa_en_kernel));
	memset(iniciar_programa,0,sizeof(t_iniciar_programa_en_kernel));

	//char * codigo = "begin\nvariables c, d\nc=1234\nd=4321\nend\0";
	char * codigo = "begin\nvariables c, d\nc=2147483647\nd=224947129\nf\nend\nfunction f\nvariables a\na=1\nend";

	iniciar_programa->codigo_de_programa = malloc(strlen(codigo)+1);
	memcpy(iniciar_programa->codigo_de_programa, codigo, strlen(codigo)+1);

	t_stream *buffer = malloc(sizeof(t_stream));

	buffer = serializar_mensaje(91,iniciar_programa);

	send(kernel_socket_descriptor,buffer->datos,buffer->size,0);

	t_header *un_header = malloc(sizeof(t_header));

	char buffer_header[5];

	int	bytes_recibidos_header,
		bytes_recibidos;

	bytes_recibidos_header = recv(kernel_socket_descriptor,buffer_header,5,MSG_PEEK);

	un_header = deserializar_header(buffer_header);

	char buffer_recibidos[un_header->length];

	bytes_recibidos = recv(kernel_socket_descriptor,buffer_recibidos,un_header->length,0);

	t_respuesta_iniciar_programa_en_kernel *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_kernel));

	respuesta = deserealizar_mensaje(92,buffer_recibidos);

	printf("Respuesta al inicio de programa: %d\n",respuesta->respuesta_correcta);


	while (1) {

			//t_header *aHeader = malloc(sizeof(t_header));

			char buffer_header[5];	//Buffer donde se almacena el header recibido

			//int bytes_recibidos_header = -1;	//Cantidad de bytes recibidos en el recv() que recibe el header
			//int bytes_recibidos= -1;			//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

			int bytes_recibidos_header = recv(kernel_socket_descriptor, buffer_header, 5,
					MSG_PEEK);

			char buffer_recv[buffer_header[1]]; //El buffer para recibir el mensaje se crea con la longitud recibida

			if (buffer_header[0] == 000000) {

//				int bytes_recibidos = recv(socket_descriptor, buffer_recv,
//						buffer_header[1], 0);
//
//				t_solicitar_bytes_de_una_pagina_a_UMC *bytes_de_una_pagina = malloc(
//						sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));
//
//				bytes_de_una_pagina =
//						(t_solicitar_bytes_de_una_pagina_a_UMC *) deserealizar_mensaje(
//								buffer_header[0], buffer_recv);
//				int pid_active = dame_pid_activo(socket_descriptor);
//				char *datos_de_lectura = leer_pagina_de_programa(pid_active,
//						bytes_de_una_pagina->pagina, bytes_de_una_pagina->offset,
//						bytes_de_una_pagina->size);
//
//				t_respuesta_bytes_de_una_pagina_a_CPU *respuesta_bytes = malloc(
//						sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));
//
//				memset(respuesta_bytes, 0,
//						sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));
//
//				respuesta_bytes->bytes_de_una_pagina = datos_de_lectura;
//
//				t_stream *buffer = (t_stream*) serializar_mensaje(32,
//						respuesta_bytes);
//
//				int bytes_sent = send(socket_descriptor, buffer->datos,
//						buffer->size, 0);
//


			}
	}

	return 0;
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
