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

#define CPULISTEN  "8002"
#define CONSOLELISTEN "8001"
#define UMCIP  "localhost"
#define UMCPORT  "21000"
#define BACKLOG 10

void *cpu_connection(int socket_descriptor);
void *console_connection(int socket_descriptor);
/*
int main(int argc, char **argv) {
	int cpu_socket_descriptor, console_socket_descriptor;
	pthread_t cpu_thread;
	pthread_t console_thread;


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
	while (1);
}
*/
void *cpu_connection(int socket_descriptor) {
	while (1) {
		int client_socket_descriptor = accept_connection(
				socket_descriptor);
		printf("Se conection una CPU");
		fflush(stdout);
	}
}

void *console_connection(int socket_desciptor) {
	while (1) {
		int client_socket_descriptor = accept_connection(socket_desciptor);
		printf("Se conectio una consola");
		fflush(stdout);
	}
}
