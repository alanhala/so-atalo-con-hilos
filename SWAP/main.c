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

int main(int argc, char *argv[]) {

	UMC_connection();

	return 0;
}

void UMC_connection() {
	int server_socket_descriptor = create_server_socket_descriptor(LISTENPORT, 10);
	while (1) {
		int umc_socket_descriptor = accept_connection(
				server_socket_descriptor);
		printf("se conecto umc");
		fflush(stdout);
	}
}

