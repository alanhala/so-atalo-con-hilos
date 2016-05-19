/*
 * main.c


 *
 *  Created on: 20/4/2016
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
#include<commons/config.h>
#include<commons/error.h>
#include<commons/log.h>
#include <commons/collections/list.h>
#include<sys/types.h>
#include<sys/inotify.h>
#include <semaphore.h>
#include "socket.h"
#include "memoriaPrincipal.h"
#include "main.h"

int BACKLOG =10;

void *kernel_and_cpu_connection_thread();
void kernel_and_cpu_connections();
void connect_to_SWAP();
void *connect_to_SWAP_thread();
void *kernel_and_cpu_connection_handler(int client_socket_descriptor);

int huboUnCambio;

#define EVENT_SIZE (sizeof (struct inotify_event) + 24)
#define BUF_LEN (1024 * EVENT_SIZE)

typedef struct umcConfigFile {
	unsigned puerto,
			puerto_swap,
			marcos,
			marcos_size,
			marco_x_proc,
			entradas_tlb,
			retardo;
	char 	*ip_swap;
} UMCConfigFile;

char* leer_string(t_config *config, char* key);

unsigned leerUnsigned(t_config *config, char* key);

void levantaConfigFileEnVariables(UMCConfigFile *ptrvaloresConfigFile,t_config *ptrConfig);
void liberaVariables(t_log* traceLogger, t_config* ptrConfig, t_log* errorLogger, t_config* ptrConfigUpdate);
void detectaCambiosEnConfigFile();
void cargar_variables_productivas(UMCConfigFile *ptrvaloresConfigFile);



int main(int argc, char **argv) {

	if (strcmp(argv[1], "-test") == 0){
		 correrTest();
		 correrTestSerializacion();
		 return 0;
	}



	connect_to_SWAP();
	kernel_and_cpu_connections();

	while (1) {
		sleep(10);
	}

	return 0;
}

void kernel_and_cpu_connections() {
	pthread_t thread;
	int thread_result = pthread_create(&thread, NULL,
			&kernel_and_cpu_connection_thread, NULL);

	if (thread_result) {
		// TODO LOGUEAR ERROR
		// TODO Analizar el tratamiento que desea darse
		printf("Error - pthread_create() return code: %d\n", thread_result);
		exit(1);
	}
}

void *kernel_and_cpu_connection_thread() {
	int server_socket_descriptor = create_server_socket_descriptor(LISTENPORT,
	BACKLOG);

	while (1) {
		int client_socket_descriptor = accept_connection(
				server_socket_descriptor);

		pthread_t thread;
		int thread_result = pthread_create(&thread, NULL,
				&kernel_and_cpu_connection_handler, client_socket_descriptor);
		if (thread_result) {
			// TODO LOGUEAR ERROR
			// TODO Analizar el tratamiento que desea darse
			printf("Error - pthread_create() return code: %d\n", thread_result);
			exit(1);
		}
		printf("Hilo creado \n"); //TODO BORRAR LINEA

	}
}

void connect_to_SWAP(){
	pthread_t thread;
	int thread_result = pthread_create(&thread, NULL,
			&connect_to_SWAP_thread, NULL);

	if (thread_result) {
		// TODO LOGUEAR ERROR
		// TODO Analizar el tratamiento que desea darse
		printf("Error - pthread_create() return code: %d\n", thread_result);
		exit(1);
	}
}

void *connect_to_SWAP_thread() {
	int swap_socket_descriptor = create_client_socket_descriptor(SWAPIP,
	SWAPPORT);
	printf("Conectado al Swap");
	fflush(stdout);
	char message[] = "hola swap, soy umc\n";

	while (1) { //TODO REVISAR por que hace print de lo que envia y de lo que recibe. puede que ser que sea por el socket?
		//char header[1];
		char recvBuffer[15];
		send(swap_socket_descriptor, "hola swap, soy umc\n", 19, 0);
		//recv(swap_socket_descriptor, header, 2, 0);
		//char recvBuffer[header];
		recv(swap_socket_descriptor, recvBuffer, 15, 0);

		printf(recvBuffer);
		fflush(stdout);
		sleep(1);
	}

}

void *kernel_and_cpu_connection_handler(int client_socket_descriptor) {
	//TODO aca deberia ir el handshake para ver si lo trato como kernel o como cpu
	char message[] = "bienvenido \n";
	while (1) {
		char recvBuffer[19];
		recv(client_socket_descriptor, recvBuffer, 19, 0);
		printf(recvBuffer);
		fflush(stdout);
		send(client_socket_descriptor, message, 12, 0);
		sleep(1);
	}
}

int cargar_configuracion(){
	t_log *errorLogger, *traceLogger;
	traceLogger = log_create("LogTraceUMC.txt","main.c",true,LOG_LEVEL_TRACE);
	errorLogger = log_create("LogErroresUMC.txt","main.c",true,LOG_LEVEL_ERROR);

	//Declaracion de Variables
	t_config *ptrConfig, *ptrConfigUpdate;
	UMCConfigFile *ptrvaloresConfigFile;
	ptrvaloresConfigFile = malloc(sizeof(UMCConfigFile));

	//Se asigna a ptrConfig el archivo de configuracion. Si no lo encuentra, finaliza
	//y lo advierte en el log
	ptrConfig = config_create("/home/utnso/GitHub/tp-2016-1c-Atalo-con-Hilos/UMC/Debug/umc.cfg");
	if (ptrConfig == NULL){
		log_error(errorLogger,"Archivo de configuración no disponible. No puede ejecutar el UMC.\n");
		return -1;
	}

	log_trace(traceLogger,"Iniciando Proceso UMC.\n");

	//El procedimiento carga los valores del Config File en las variables creadas
	levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfig);

	log_trace(traceLogger,"Archivo de Configuracion levantado exitosamente.\n");



	/*

	while(1)
	{
		huboUnCambio=0;
		detectaCambiosEnConfigFile();
		if(huboUnCambio)
		{
			ptrConfigUpdate = config_create("/home/utnso/GitHub/tp-2016-1c-Atalo-con-Hilos/UMC/umc.cfg");
			if(ptrConfigUpdate->properties->elements_amount==0) {
				log_error(errorLogger,"No se puede levantar el Archivo de Configuracion.\n");
			} else {
				levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfigUpdate);
				printf("%s\n\n", ptrvaloresConfigFile->ip_swap);
				log_trace(traceLogger,"Archivo de Configuracion actualizado y levantado.\n");
			}
			config_destroy(ptrConfigUpdate);
		}
	}
	*/
	cargar_variables_productivas(ptrvaloresConfigFile);
	//printf("%s\n\n", ptrvaloresConfigFile->ip_swap);

	liberaVariables(traceLogger, ptrConfig, errorLogger, ptrConfigUpdate);
	free(ptrvaloresConfigFile);

	return 0;

};


unsigned leerUnsigned(t_config *config, char* key) {
	unsigned datoInt = 0;
	if (config_has_property(config, key)) {
		datoInt = config_get_int_value(config, key);
	} else {
		error_show("No se leyó el %s de la config \n", key);
	}

	return datoInt;
}
void levantaConfigFileEnVariables(UMCConfigFile *ptrvaloresConfigFile,t_config *ptrConfig){
	ptrvaloresConfigFile->puerto = leerUnsigned(ptrConfig, "PUERTO");
	ptrvaloresConfigFile->puerto_swap = leerUnsigned(ptrConfig, "PUERTO_SWAP");
	ptrvaloresConfigFile->marcos = leerUnsigned(ptrConfig, "MARCOS");
	ptrvaloresConfigFile->marcos_size = leerUnsigned(ptrConfig, "MARCOS_SIZE");
	ptrvaloresConfigFile->marco_x_proc = leerUnsigned(ptrConfig, "MARCO_X_PROC");
	ptrvaloresConfigFile->entradas_tlb = leerUnsigned(ptrConfig, "ENTRADAS_TLB");
	ptrvaloresConfigFile->retardo = leerUnsigned(ptrConfig, "RETARDO");
	ptrvaloresConfigFile->ip_swap = leer_string(ptrConfig, "IP_SWAP");

}
void liberaVariables(t_log* traceLogger, t_config* ptrConfig, t_log* errorLogger, t_config* ptrConfigUpdate) {
	//Libera Logs y el Config File
	log_trace(traceLogger, "Se libera el Archivo de Configuracion.\n");
	config_destroy(ptrConfig);
	//config_destroy(ptrConfigUpdate); // TODO MKN EZE
	log_trace(traceLogger, "Se libera el Log de Errores.\n");
	log_destroy(errorLogger);
	log_trace(traceLogger, "Se libera el Trace Log.\n");
	log_destroy(traceLogger);
}
void detectaCambiosEnConfigFile() {
		char buffer[BUF_LEN];
		// Al inicializar inotify este nos devuelve un descriptor de archivo
		int file_descriptor = inotify_init();
		if (file_descriptor < 0) {
			perror("inotify_init");
		}
		// Creamos un monitor sobre un path indicando que eventos queremos escuchar
		int watch_descriptor = inotify_add_watch(file_descriptor,
				"/home/utnso/GitHub/tp-2016-1c-Atalo-con-Hilos/UMC/Debug", IN_MODIFY);

		// El file descriptor creado por inotify, es el que recibe la información sobre los eventos ocurridos
		// para leer esta información el descriptor se lee como si fuera un archivo comun y corriente pero
		// la diferencia esta en que lo que leemos no es el contenido de un archivo sino la información
		// referente a los eventos ocurridos
		int length = read(file_descriptor, buffer, BUF_LEN);
		if (length < 0) {
			error_show("Error al leer el descriptor de archivo");
		}
		int offset = 0;

		// Luego del read buffer es un array de n posiciones donde cada posición contiene
		// un eventos ( inotify_event ) junto con el nombre de este.
		while (offset < length) {

			// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
			// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
			// a sizeof( struct inotify_event ) + 24.
			struct inotify_event *event = (struct inotify_event *) &buffer[offset];
			// El campo "len" nos indica la longitud del tamaño del nombre
			if (event->len) {
				// Dentro de "mask" tenemos el evento que ocurrio y sobre donde ocurrio
				// sea un archivo o un directorio
				if (event->mask & IN_MODIFY) {
					if (event->mask) {
						printf("The file %s was modified.\n\n", event->name);
						huboUnCambio = 1;
					}
					}
				}
			offset += sizeof(struct inotify_event) + event->len;
			}

		inotify_rm_watch(file_descriptor, watch_descriptor);
		close(file_descriptor);
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

}

char* leer_string(t_config *config, char* key) {
	char * datoString = malloc(sizeof(config_get_string_value(config, key)));
	if (config_has_property(config, key)) {
		datoString = config_get_string_value(config, key);
	} else {
		error_show("No se leyó el %s de la config \n", key);
	}
	return datoString;
}
