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

#define PORT "25005"
#define BACKLOG 10
#define MAXDATASIZE 100
#define MYIP "192.168.1.35" // TODO es necesario?

int createServerSocketReadyToAccept(struct sockaddr_in* my_addr,
		int serverSocketfd);

void *connectionResponse(int *clientSock_fd);

void *handshake(void *param);

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

int main(int argc, char **argv) {

	int serverSocketfd, newSock_fd;
	//struct sockaddr_in my_addr;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information

	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((serverSocketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(serverSocketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(serverSocketfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(serverSocketfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(serverSocketfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	//serverSocketfd = createServerSocketReadyToAccept(&my_addr, serverSocketfd);

	while (1) {
		newSock_fd = accept(serverSocketfd, (struct sockaddr *)&their_addr, &sin_size);
		if (newSock_fd == -1) {
			// TODO Loguear error de aceptacion
			// TODO analizar el tratamiento que quiere darse
			printf("server: error al aceptar cliente...\n");
			continue;
		}
		//void *ptr; //TODO analizar si hay problema
		//connectionResponse(newSock_fd);
		printf("server: cliente aceptado\n");

	}

	return 0;
}
/*
int createServerSocketReadyToAccept(struct sockaddr_in* my_addr,
		int serverSocketfd) {
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(PORT);
	my_addr->sin_addr.s_addr = inet_addr(MYIP);
	memset(&(my_addr->sin_zero), '\0', 8); // Poner a cero el resto de la estructura
	serverSocketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocketfd == -1) {
		// TODO Loguear que no se pudo crear el socket
		// TODO Analizar el tratamiento que quiere darse
		exit(1);
	}
	// TODO Loguear que se creo el socket
	printf("El socket se creo correctamente");
	//bind(sockfd, res->ai_addr, res->ai_addrlen);
	//if (bind(serverSocketfd, my_addr->sin_addr.s_addr, my_addr->) == -1) {
		//if (bind(serverSocketfd, struct sockaddr*) &*my_addr, sizeof(struct sockaddr)) == -1) {
	if(0){
	printf("error de bindeo");
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


void *connectionResponse(int *clientSock_fd) {

	printf("Llego al connection response"); //TODO BORRAR LINEA
	pthread_t thread;

	int iret1 = pthread_create(&thread, NULL, &handshake, clientSock_fd);
	//Cuarto Parametro:
	//*arg - pointer to argument of function.
	//To pass multiple arguments, send a pointer to a structure.

	if (iret1) {
		// TODO LOGUEAR ERROR
		// TODO Analizar el tratamiento que desea darse
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
		exit(1);
	}

	pthread_join(thread, NULL);

	exit(EXIT_SUCCESS); //TODO exit_success??

}

void *handshake(void *param) {
	printf("llego al handshake"); //TODO borrar linea
}
*/
