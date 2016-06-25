/*
 * main.c


 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <commons/error.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <sys/types.h>
#include <semaphore.h>
#include "socket.h"
#include "memoriaPrincipal.h"
#include "protocoloUMC.h"
#include "main.h"

t_log 	*trace_log_UMC;

int BACKLOG =10;

void *kernel_and_cpu_connection_handler(int client_socket_descriptor);
void *interprete_comando_thread();

int huboUnCambio;

void interprete_de_comandos();
void manejo_de_solicitudes(int cpu_socket_descriptor);

int main(int argc, char **argv) {

	//if (strcmp(argv[1], "-test") == 0 || strcmp(argv[1], "-testMock") == 0){
//		if(strcmp(argv[1], "-testMock") == 0)
	//		set_test();//para usar mock
		//simulaciones();

		//correr_swap_mock_test();

		//correrTest();
		//correrTestSerializacion();
	//}

	 trace_log_UMC = log_create("Log_de_UMC.txt",
								"main.c",
								false,
								LOG_LEVEL_TRACE);

	pthread_t interprete_comandos;
	int interprete_thread_result = pthread_create(&interprete_comandos, NULL,
			&interprete_comando_thread, NULL);

	pthread_t levanta_config_file;
	int levanta_config_file_resultado = pthread_create(&levanta_config_file,NULL,&cargar_configuracion,NULL);

	if (interprete_thread_result) {
		log_trace(trace_log_UMC,"Error en la creacion del thread del interprete de comandos\n. Codigo de error: %d", interprete_thread_result);
		// TODO Analizar el tratamiento que desea darse
		exit(1);
	}

	set_algoritmo_reemplazo("clock");

	inicializar_estructuras();
	//int swap_socket = create_client_socket_descriptor("localhost", "6000");
	//set_socket_descriptor(swap_socket);
	set_test();
	crear_swap_mock();


	int server_socket_descriptor = create_server_socket_descriptor("localhost","5000",BACKLOG);


	while (1) {
			int client_socket_descriptor = accept_connection(server_socket_descriptor);

			pthread_t thread;
			int thread_result = pthread_create(&thread, NULL,
					&kernel_and_cpu_connection_handler, client_socket_descriptor);
			if (thread_result) {
				log_trace(trace_log_UMC,"Error en la creacion del thread que maneja las conexiones de CPU y Nucleo\n. Codigo de error: %d\n",thread_result);
				// TODO Analizar el tratamiento que desea darse
				exit(1);
			}
		}


		while (1) {
			sleep(10);
		}

		return 0;
}


void *kernel_and_cpu_connection_handler(int client_socket_descriptor) {
	manejo_de_solicitudes(client_socket_descriptor);
	return 0;
}

void manejo_de_solicitudes(int socket_descriptor) {
	int handshake = -1;
	recv(socket_descriptor, &handshake, sizeof(int), 0);
	if(handshake == 1) //CPU
	{
		t_cpu_context * nueva_cpu = malloc(sizeof(t_cpu_context));
		nueva_cpu->cpu_id = socket_descriptor;
		nueva_cpu->pid_active = -1;
		list_add(lista_cpu_context, nueva_cpu);
	}
	if(handshake == 2) //KERNEL
	{
		//cargar_nuevo_programa(1, 50, "begin\nvariables c, d\nc=2147483647\nd=224947129\nend\0");
		//cargar_nuevo_programa(1, 50, "begin\nvariables c, d\nc=1234\nd=4321\nend\0");
		//cargar_nuevo_programa(2, 50, "cargo un programa");
	}

	while (1) {

		t_header *a_header = malloc(sizeof(t_header));

		char buffer_header[5];	//Buffer donde se almacena el header recibido

		//int bytes_recibidos_header = -1;	//Cantidad de bytes recibidos en el recv() que recibe el header
		//int bytes_recibidos= -1;			//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

		int bytes_recibidos_header = recv(socket_descriptor, buffer_header, 5,
				MSG_PEEK);

		a_header = deserializar_header(buffer_header);

		char buffer_recv[a_header->length]; //El buffer para recibir el mensaje se crea con la longitud recibida

		int tipo = a_header->tipo;
		int length = a_header->length;

		if (tipo == 31) {

			int bytes_recibidos = recv(socket_descriptor, buffer_recv,
				length, 0);

			t_solicitar_bytes_de_una_pagina_a_UMC *bytes_de_una_pagina = malloc(
					sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));

			bytes_de_una_pagina =
					(t_solicitar_bytes_de_una_pagina_a_UMC *) deserealizar_mensaje(
							tipo, buffer_recv);

			int pid_active = dame_pid_activo(socket_descriptor);
			char *datos_de_lectura = leer_pagina_de_programa(pid_active,
					bytes_de_una_pagina->pagina, bytes_de_una_pagina->offset,
					bytes_de_una_pagina->size);

			t_respuesta_bytes_de_una_pagina_a_CPU *respuesta_bytes = malloc(
					sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));

			memset(respuesta_bytes, 0,
					sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));

			respuesta_bytes->bytes_de_una_pagina = datos_de_lectura;

			t_stream *buffer = (t_stream*) serializar_mensaje(32,
					respuesta_bytes);

			int bytes_sent = send(socket_descriptor, buffer->datos,
					buffer->size, 0);
		}

		if (tipo == 33) {

			int bytes_recibidos = recv(socket_descriptor, buffer_recv,
					length, 0);

			t_escribir_bytes_de_una_pagina_en_UMC *bytes_de_una_pagina = malloc(
					sizeof(t_escribir_bytes_de_una_pagina_en_UMC));

			bytes_de_una_pagina =
					(t_escribir_bytes_de_una_pagina_en_UMC *) deserealizar_mensaje(
							buffer_header[0], buffer_recv);
			int pid_active = dame_pid_activo(socket_descriptor);
			int estado_escritura = escribir_pagina_de_programa(pid_active,
					bytes_de_una_pagina->pagina, bytes_de_una_pagina->offset,
					bytes_de_una_pagina->size, bytes_de_una_pagina->buffer);

			t_respuesta_escribir_bytes_de_una_pagina_en_UMC *respuesta_escritura =
					malloc(
							sizeof(t_respuesta_escribir_bytes_de_una_pagina_en_UMC));

			respuesta_escritura->escritura_correcta = estado_escritura;

			t_stream *buffer = (t_stream*) serializar_mensaje(34,
					respuesta_escritura);

			int bytes_sent = send(socket_descriptor, buffer->datos,
					buffer->size, 0);
		}
		if (tipo == 35) {

			int bytes_recibidos = recv(socket_descriptor, buffer_recv,
					length, 0);

			t_cambio_de_proceso *cambio_de_proceso = malloc(
					sizeof(t_cambio_de_proceso));

			cambio_de_proceso = (t_cambio_de_proceso *) deserealizar_mensaje(
					buffer_header[0], buffer_recv);
			int respuesta_temp = cambio_contexto(socket_descriptor,
					cambio_de_proceso->pid);

			t_respuesta_cambio_de_proceso *respuesta_c_de_proceso = malloc(
					sizeof(t_respuesta_cambio_de_proceso));

			respuesta_c_de_proceso->cambio_correcto = respuesta_temp;

			t_stream *buffer = (t_stream*) serializar_mensaje(36,
					respuesta_c_de_proceso);

			int bytes_sent = send(socket_descriptor, buffer->datos,
					buffer->size, 0);
		}
		if(tipo == 61){

			int bytes_recibidos = recv(socket_descriptor,buffer_recv,length,0);

			t_inicio_de_programa_en_UMC *inicio_programa_en_UMC = malloc(sizeof(t_inicio_de_programa_en_UMC));

			inicio_programa_en_UMC = (t_inicio_de_programa_en_UMC *)deserealizar_mensaje(buffer_header[0],buffer_recv);

			int respuesta_tmp = cargar_nuevo_programa(inicio_programa_en_UMC->process_id,
						   	   	   	   	   	   	   	   	   	 inicio_programa_en_UMC->cantidad_de_paginas,
															 inicio_programa_en_UMC->codigo_de_programa);

			t_respuesta_inicio_de_programa_en_UMC *respuesta = malloc(sizeof(t_respuesta_inicio_de_programa_en_UMC));

			respuesta->respuesta_correcta = respuesta_tmp;

			t_stream *buffer = (t_stream*)serializar_mensaje(62,respuesta);

			int bytes_sent = send(socket_descriptor,buffer->datos,buffer->size,0);

			   }
		if(tipo==63) {

			int bytes_recibidos = recv(socket_descriptor,buffer_recv,length,0);

			t_finalizar_programa_en_UMC *finalizar_programa_en_UMC = malloc(sizeof(t_finalizar_programa_en_UMC));

			finalizar_programa_en_UMC = (t_finalizar_programa_en_UMC *)deserealizar_mensaje(63,buffer_recv);

			t_respuesta_finalizar_programa_en_UMC *respuesta = malloc(sizeof(t_respuesta_finalizar_programa_en_UMC));

			//TODO Setear este valor
			int respuesta_tmp = 1;
			respuesta->respuesta_correcta = respuesta_tmp;

			t_stream *buffer = (t_stream *)serializar_mensaje(64,respuesta);

			int bytes_sent = send(socket_descriptor,buffer->datos,buffer->size,0);

		}

	}
}

void* interprete_comando_thread(){
	while(1)
	{
		extern t_log *trace_log_UMC;

		bool respuesta_invalida = true;

		char *comando = malloc(100);
		char **comando_separado = malloc(100);
		int numero_del_comando;
		int i = 0;

		char *comandos_validos[5]={"retardo","dumpstruct","dumpmemory","flushmemory","flushtlb"};

		puts("Ingrese un comando:");
		fgets(comando, 100,stdin);
		comando_separado = string_split(comando," ");

		while(respuesta_invalida){
			for(i=0;i<=4;i++){
				if(!strcasecmp(comando_separado[0],comandos_validos[i])){
					numero_del_comando = atoi(comando_separado[1]);
					respuesta_invalida = false;
					break;
				}
			}
			if(!respuesta_invalida)
				break;

			puts("Entrada no valida");
			puts("Ingrese un comando:");
			fgets(comando, 100,stdin);
			comando_separado = string_split(comando," ");
		}

		if(!strcasecmp("retardo", comando_separado[0])){
			set_retardo(numero_del_comando);
		} else if (!strcasecmp("dumpstruct", comando_separado[0])){
			dump_structs(numero_del_comando);
		} else if (!strcasecmp("dumpmemory", comando_separado[0])){
			dump_memory(numero_del_comando);
		} else if (!strcasecmp("flushmemory", comando_separado[0])){
			flush_memory(numero_del_comando);
		} else if (!strcasecmp("flushtlb", comando_separado[0])){
			flush_tlb(numero_del_comando);
		}

		free(comando_separado);
		free(comando);
	}

}

void cargar_variables_productivas(UMCConfigFile *config){
	//cargo un string - inicio
	SWAPIP = malloc(strlen(config->ip_swap)+1);
	memset(&SWAPIP, 0, (strlen(config->ip_swap)+1));
	SWAPIP = config->ip_swap;
	//cargo un string - fin

	LISTENPORT= config->puerto;
	SWAPPORT= config->puerto_swap;

	set_cantidad_entradas_tlb(config->entradas_tlb);
	set_max_frames_por_proceso(config->marco_x_proc);
	set_cantidad_frames(config->marcos);
	set_tamanio_frame(config->marcos_size);
	set_retardo(config->retardo);
	//set_algoritmo_reemplazo(config->algoritmo_reemplazo);
};
