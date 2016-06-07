/*
 * kernel_main.c
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
#include "socket.h"
#include "tests.h"
#include "protocoloKernel.h"
#include "nucleo.h"

#define CPULISTEN  "8002"
#define CONSOLELISTEN "8001"
#define UMCIP  "localhost"
#define UMCPORT  "5000"
#define BACKLOG 10

void *cpu_connection(int socket_descriptor);
//void *console_connection(int socket_descriptor);
void console_connection(int socket_descriptor);

int main(int argc, char **argv) {

	iniciar_algoritmo_planificacion();


	int console_socket_descriptor = create_server_socket_descriptor(NULL,"22000",10);
	console_connection(console_socket_descriptor);
	/*
	int umc_socket_descriptor = create_client_socket_descriptor("localhost", "5000");

	int a =2;
	send(umc_socket_descriptor, &a, sizeof(int), 0);
	printf("conectado con umc y mande handshake");
	fflush(stdout);

	*/


	/*

	Conexion Inicio de Programa con UMC -- Inicio

    t_inicio_de_programa_en_UMC *iniciar_programa_en_UMC = malloc(sizeof(t_inicio_de_programa_en_UMC));
	   memset(iniciar_programa_en_UMC,0,sizeof(t_inicio_de_programa_en_UMC));

	   iniciar_programa_en_UMC->process_id = 22;
	   iniciar_programa_en_UMC->cantidad_de_paginas = 4;
	   iniciar_programa_en_UMC->codigo_de_programa = malloc(5);
	   char unChar[5] = "Hola";
	   memcpy(iniciar_programa_en_UMC->codigo_de_programa,&unChar,5);

	   t_stream *buffer = malloc(sizeof(t_stream));

	   buffer = serializar_mensaje(61,iniciar_programa_en_UMC);

	   int bytes_enviados = send(umc_socket_descriptor,buffer->datos,buffer->size,0);

	   char buffer_header[5];

	   int bytes_header = recv(umc_socket_descriptor,buffer_header,5,MSG_PEEK);

	   char buffer_recv[buffer_header[1]];

	   int bytes_recibidos = recv(umc_socket_descriptor,buffer_recv,buffer_header[1],0);

	   t_respuesta_iniciar_programa_en_UMC *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_UMC));
	   memset(respuesta,0,sizeof(t_respuesta_iniciar_programa_en_UMC));
	   respuesta = deserealizar_mensaje(buffer_header[0],buffer_recv);

	Conexion Inicio de Programa con UMC -- Fin

	*/



	/*

	Conexion Finalizar Programa con UMC -- Inicio

	t_finalizar_programa_en_UMC *finalizar_programa_en_UMC = malloc(sizeof(t_finalizar_programa_en_UMC));
	memset(finalizar_programa_en_UMC,0,sizeof(t_finalizar_programa_en_UMC));

	//HARDCODEADO. ESTA MAL
	int pid_temp = 22;
	finalizar_programa_en_UMC->process_id = pid_temp;

	t_stream *buffer = malloc(sizeof(t_stream));

	buffer = serializar_mensaje(63,finalizar_programa_en_UMC);

	int bytes_enviados = send(umc_socket_descriptor,buffer->datos,buffer->size,0);

	char buffer_header[5];

	int bytes_header = recv(umc_socket_descriptor,buffer_header,5,MSG_PEEK);

	char buffer_recv[buffer_header[1]];

	int bytes_recibidos = recv(umc_socket_descriptor,buffer_recv,buffer_header[1],0);

	t_respuesta_finalizar_programa_en_UMC *respuesta_finalizar_prog_UMC = malloc(sizeof(t_respuesta_finalizar_programa_en_UMC));

	memset(respuesta_finalizar_prog_UMC,0,sizeof(t_respuesta_finalizar_programa_en_UMC));

	respuesta_finalizar_prog_UMC = deserealizar_mensaje(64,buffer_recv);

	return respuesta_finalizar_prog_UMC;

	Conexion Finalizar Programa con UMC -- Fin

	*/

	/*
	while(1){
		sleep(1000);
	}


	if (strcmp(argv[1], "-test") == 0){
		 correrTest();
		 return 0;
	}
	if (strcmp(argv[1], "-planificador") == 0){
		 Planificacion();
		 return 0;
	}
	*/


	//int cpu_socket_descriptor, console_socket_descriptor;
	//pthread_t cpu_thread;
	//pthread_t console_thread;

	/*
	cpu_socket_descriptor = create_server_socket_descriptor(CPULISTEN, 4);
	if (pthread_create(&cpu_thread, NULL, &cpu_connection, cpu_socket_descriptor)) {
		printf("No se pudo crear el thread");
		fflush(stdout);
		exit(1);
	}
	console_socket_descriptor = create_server_socket_descriptor(CONSOLELISTEN, 4);
	if (pthread_create(&console_thread, NULL, &console_connection, console_socket_descriptor)) {
		printf("No se pudo crear el thread");
		fflush(stdout);
		exit(1);
	}

	while (1); */
	return 0;
}

void *cpu_connection(int socket_descriptor) {
	while (1) {
		int client_socket_descriptor = accept_connection(
				socket_descriptor);
		//printf("Se conection una CPU");
		//fflush(stdout);
	}

}
/*
void *umc_connection(int socket_descriptor) {
	while (1) {
		int client_socket_descriptor = accept_connection(
				socket_descriptor);
		//printf("Se conection una CPU");
		//fflush(stdout);

		t_header *aHeader = malloc(sizeof(t_header));

		char 	buffer_header[5];	//Buffer donde se almacena el header recibido

		int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
				bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

		bytes_recibidos_header = recv(client_socket_descriptor, buffer_header, 5, MSG_PEEK);

		char buffer_recv[buffer_header[1]]; 	//El buffer para recibir el mensaje se crea con la longitud recibida

		if (buffer_header[0] == 1)
	}

}
*/
//void *console_connection(int socket_desciptor) {
void console_connection(int socket_desciptor) {
	//while (1) {
		int client_socket_descriptor = accept_connection(socket_desciptor);


		t_header *un_header = malloc(sizeof(t_header));
		char buffer_header[5];

		int	bytes_recibidos_header,
			bytes_recibidos;

		bytes_recibidos_header = recv(client_socket_descriptor,buffer_header,5,MSG_PEEK);

		un_header = deserializar_header(buffer_header);

		char buffer_recibidos[(un_header->length)];

		if(un_header->tipo == 91){

			int bytes_recibidos = recv(client_socket_descriptor,buffer_recibidos,un_header->length,0);

			t_iniciar_programa_en_kernel *iniciar_programa = malloc(sizeof(t_iniciar_programa_en_kernel));

			iniciar_programa = (t_iniciar_programa_en_kernel *)deserealizar_mensaje(91,buffer_recibidos);

			printf("Kernel. El mensaje es: %s\n",iniciar_programa->codigo_de_programa);
			printf("Kernel. El mensaje tiene de largo: %d\n",un_header->length);

			// AGREGO EL CODIGO A LA COLA DE NEW
			char * codigo_programa = iniciar_programa->codigo_de_programa;
			sem_wait(&mut_new);
			queue_push(estado_new, codigo_programa);
			sem_post(&mut_new);
			sem_post(&cant_new);
			// FIN

			t_respuesta_iniciar_programa_en_kernel *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_kernel));
			memset(respuesta,0,sizeof(t_respuesta_iniciar_programa_en_kernel));

			uint32_t respuesta_tmp = 22;
			respuesta->respuesta_correcta=respuesta_tmp;

			t_stream *buffer = malloc(sizeof(t_stream));

			buffer = serializar_mensaje(92,respuesta);

			int bytes_sent = send(client_socket_descriptor,buffer->datos,buffer->size,0);

		}
	}
//}
