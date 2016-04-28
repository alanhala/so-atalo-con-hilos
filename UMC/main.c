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

void kernel_and_cpu_connections();
void connect_to_SWAP();

int main(int argc, char **argv) {

	connect_to_SWAP();

	while (1) {
		sleep(10);
	}

	return 0;
}

void kernel_and_cpu_connections() {
	int server_socket_descriptor = create_server_socket_descriptor(LISTENPORT,
			BACKLOG);
	while (1) {
		int client_socket_descriptor = accept_connection(
				server_socket_descriptor);
	}
}

void connect_to_SWAP() {
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
