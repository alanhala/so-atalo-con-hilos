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

#define PORT "25000"
#define BACKLOG 10
#define MAXDATASIZE 100
#define MYIP "192.168.1.35" // TODO es necesario?

int createServerSocketReadyToAccept(struct sockaddr_in* my_addr,
		int serverSocketfd);

int main(int argc, char **argv) {

	int serverSocketfd, newSock_fd;
	struct sockaddr_in my_addr;
	struct sockaddr_storage their_addr; // connector's address information

	serverSocketfd = createServerSocketReadyToAccept(&my_addr, serverSocketfd);

	while (1) {
		newSock_fd = accept(serverSocketfd, (struct sockaddr *) &their_addr, sizeof(their_addr));
		if (newSock_fd == -1) {
			// TODO Loguear error de aceptacion
			// TODO analizar el tratamiento que quiere darse
			exit(1);
		}
		else{
			void *ptr; //TODO analizar si hay problema
			creoThreads(ptr);
		}
	}

	return 0;
}

int createServerSocketReadyToAccept(struct sockaddr_in* my_addr,
		int serverSocketfd) {
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = inet_addr(MYIP);
	memset(&(my_addr->sin_zero), '\0', 8); // Poner a cero el resto de la estructura
	serverSocketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocketfd == -1) {
		// TODO Loguear que no se pudo crear el socket
		// TODO Analizar el tratamiento que quiere darse
		exit(1);
	}
	// TODO Loguear que se creo el socket
	printf("El socket se creo correctamente");
	if (bind(serverSocketfd, (struct sockaddr*) &*my_addr,
			sizeof(struct sockaddr)) == -1) {
		close(serverSocketfd);
		//TODO loguear el error
		//TODO analizar el tratamiento que hay que darle
		exit(1);
	}
	// TODO Loguear que se bindeo correctamente el socket

	if (listen(serverSocketfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);

	}
	// TODO Loguear esperando conexiones
	printf("server: waiting for connections...\n");
	return serverSocketfd;

}
