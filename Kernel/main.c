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
#include "levanta_config_file.h"
#include "main.h"

t_log 	*kernel_trace;


void *cpu_connection(int socket_descriptor);
void *console_and_cpu_connection_handler(int client_socket_descriptor);
void inicializa_semaforos_kernel(void);

int main(int argc, char **argv) {
	//todo me gustaria implementar  el archivo de configuracion asi empiezo a usarlo directamente
	//pero me tira un par de errores cuando descomento las lineas que uso gkernel
	//gkernel = create_kernel(CONFIGPATH);
	//set_page_size(5);

	inicializa_semaforos_kernel();
	kernel_trace= log_create("./Kernel_trace.txt",
										"main.c",
										true,
										LOG_LEVEL_TRACE);


	log_trace(kernel_trace,"\n\n\n\n\n");

	t_kernel *kernel = create_kernel("./kernel_config.txt");

	pthread_t levanta_config_file;
	int levanta_config_file_resultado = pthread_create(&levanta_config_file,NULL,&cargar_configuracion,kernel);


	sem_wait(&sem_config_file_kernel);
	int umc_fd = create_client_socket_descriptor(umc_ip,puerto_umc);
	scheduler->umc_socket_descriptor = umc_fd;
	int a =2;
	send(umc_fd, &a, sizeof(int), 0);
	int tamanio_pagina = -1;
	recv(umc_fd, &tamanio_pagina, sizeof(int), 0);
	set_page_size(tamanio_pagina);

	int server_socket_descritptor =  create_server_socket_descriptor(server_ip,server_port,backlog);

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
		sem_wait(&mutex_cpus_available);
		int cpu_socket_descriptor = client_socket_descriptor;
		queue_push(scheduler->cpus_available, cpu_socket_descriptor);
		sem_post(&mutex_cpus_available);
		sem_post(&sem_cpus_available);
	}
	if(handshake == 2) //Consola
	{
		printf("consola conectada\n");

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
			t_new_program* new_program = malloc(sizeof(t_new_program));
			new_program->program_code = iniciar_programa->codigo_de_programa;
			new_program->console_socket_descriptor = client_socket_descriptor;
			//char * codigo_programa = iniciar_programa->codigo_de_programa;
			sem_wait(&mutex_new);
			queue_push(scheduler->new_state, new_program);
			sem_post(&mutex_new);
			sem_post(&sem_new);
			// FIN

			t_respuesta_iniciar_programa_en_kernel *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_kernel));
			memset(respuesta,0,sizeof(t_respuesta_iniciar_programa_en_kernel));

			uint32_t respuesta_tmp = 22;
			respuesta->respuesta_correcta=respuesta_tmp;

			t_stream *buffer = malloc(sizeof(t_stream));

			buffer = serializar_mensaje(92,respuesta);

			int bytes_sent = send(client_socket_descriptor,buffer->datos,buffer->size,0);
			free(buffer->datos);
			free(buffer);
			int signal_received;
			recv(client_socket_descriptor, &signal_received, sizeof(int) , 0);
			if (signal_received == 1) {
				list_add(scheduler->closed_consoles, client_socket_descriptor);
			}
		}
	}
}

void inicializa_semaforos_kernel(void){
	sem_init(&sem_config_file_kernel,0,0);
}
