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

#define LISTENPORT "21000"
#define SWAPPORT "20000"
#define SWAPIP "localhost" // TODO es necesario?

void kernel_and_cpu_connections();
void connect_to_SWAP();

int main(int argc, char **argv) {

	void kernel_and_cpu_connections();
	void connect_to_SWAP();

	return 0;
}

void kernel_and_cpu_connections() {
	int server_socket_descriptor = create_server_socket_descriptor("",
	LISTENPORT);
	while (1) {
		int client_socket_descriptor = accept_connection(
				server_socket_descriptor);
	}
}

void connecto_to_SWAP() {

}
