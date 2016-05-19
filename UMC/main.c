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
#include "socket.h"

#define LISTENPORT "21000"
#define SWAPPORT "8000"
#define SWAPIP "localhost" // TODO es necesario?
#define BACKLOG 10

void *kernel_and_cpu_connection_thread();
void kernel_and_cpu_connections();
void connect_to_SWAP();
void *connect_to_SWAP_thread();
void *kernel_and_cpu_connection_handler(int client_socket_descriptor);

int main(int argc, char **argv) {

	if (strcmp(argv[1], "-test") == 0){
//		 correrTest();
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
