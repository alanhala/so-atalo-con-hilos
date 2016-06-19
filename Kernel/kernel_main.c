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
#include "protocoloKernel.h"
#include "kernel.h"
#include "nucleo.h"

#define CPULISTEN  "8000"
#define UMCIP  "localhost"
#define UMCPORT  "5000"
#define CONFIGPATH "kernel_config.txt"
#define BACKLOG 10
//t_kernel * gkernel;

void *cpu_connection(int socket_descriptor);
void *console_and_cpu_connection_handler(int client_socket_descriptor);

int main(int argc, char **argv) {
	//todo me gustaria implementar  el archivo de configuracion asi empiezo a usarlo directamente
	//pero me tira un par de errores cuando descomento las lineas que uso gkernel
	//gkernel = create_kernel(CONFIGPATH);
	iniciar_algoritmo_planificacion();

	int umc_fd = create_client_socket_descriptor("localhost", "5000");
	set_umc_socket_descriptor(umc_fd);
	int a =2;
	send(umc_fd, &a, sizeof(int), 0);


	int server_socket_descritptor =  create_server_socket_descriptor("localhost","9000",10);

	while(1){
		int client_socket_descriptor = accept_connection(server_socket_descritptor);


		pthread_t thread;
		int thread_result = pthread_create(&thread, NULL,
				&console_and_cpu_connection_handler, client_socket_descriptor);
		if (thread_result) {
			// TODO LOGUEAR ERROR
			// TODO Analizar el tratamiento que desea darse
			printf("Error - pthread_create() return code: %d\n", thread_result);
			exit(1);
		}


	}

	return 0;
}


void *console_and_cpu_connection_handler(int client_socket_descriptor) {
	manejo_de_solicitudes(client_socket_descriptor);
	return 0;
}


void manejo_de_solicitudes(int client_socket_descriptor) {
	int handshake = -1;
	recv(client_socket_descriptor, &handshake, sizeof(int), 0);
	if(handshake == 1) //CPU
	{
		printf("cpu conectada\n");
		sem_wait(&mut_cpu_disponibles);
		int cpu_socket_descriptor = client_socket_descriptor;
		queue_push(cola_cpu_disponibles, cpu_socket_descriptor);
		sem_post(&mut_cpu_disponibles);
		sem_post(&cant_cpu_disponibles);
	}
	if(handshake == 2) //Consola
	{
		printf("consola conectada\n");
	}


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
				t_new_program * nuevo_programa = malloc(sizeof(t_new_program));
				nuevo_programa->codigo_programa = iniciar_programa->codigo_de_programa;
				nuevo_programa->console_socket_descriptor = client_socket_descriptor;
				//char * codigo_programa = iniciar_programa->codigo_de_programa;
				sem_wait(&mut_new);
				queue_push(estado_new, nuevo_programa);
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

