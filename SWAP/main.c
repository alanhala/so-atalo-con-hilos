/*
 * main.c
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

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

#define LISTENPORT "8000"
#define BACKLOG 10

void UMC_connection();
void *UMC_connection_handler(int umc_socket_descriptor);

int main(int argc, char *argv[]) {

	UMC_connection();
	while (1) {
		//TODO preguntar a ayudante que hago para que no finalice si los threads estan abiertos
		sleep(10);
	}
	return 0;
}

void UMC_connection() {
	//Solo se va a conectar un proceso UMC

	int server_socket_descriptor = create_server_socket_descriptor(LISTENPORT,
			10);
	int umc_socket_descriptor = accept_connection(server_socket_descriptor);

	pthread_t thread;
	int thread_result = pthread_create(&thread, NULL, &UMC_connection_handler,
			umc_socket_descriptor);
	//TODO preguntar a ayudante que pasa aca con los argumentos

	if (thread_result) {
		// TODO LOGUEAR ERROR
		// TODO Analizar el tratamiento que desea darse
		printf("Error - pthread_create() return code: %d\n", thread_result);
		exit(1);
	}
	printf("Hilo creado \n"); //TODO BORRAR LINEA

}

void *UMC_connection_handler(int umc_socket_descriptor) {
	printf("se conecto umc\n");
	fflush(stdout);
	char message[] = "bienvenido umc\n";
	while (1) {
		char recvBuffer[19];
		recv(umc_socket_descriptor, recvBuffer, 19, 0);
		printf(recvBuffer);
		fflush(stdout);
		send(umc_socket_descriptor, message, 15, 0);
		sleep(1);
	}
}
