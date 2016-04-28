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

#define KERNELLISTENPORT  "30000"
#define UMCIP  "localhost"
#define UMCPORT  "21000"

void console_and_cpu_connections();

int main(int argc, char **argv) {
	void console_and_cpu_connections();
	return 0;
}

void console_and_cpu_connections() {
	int server_socket_descriptor = create_server_socket_descriptor("",
			KERNELLISTENPORT);
	while (1) {
		int client_socket_descriptor = accept_connection(
				server_socket_descriptor);
	}
}

