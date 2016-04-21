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

static int threadsActive = 0;
void *connectionResponse(int *clientSock_fd);

void *handshake(void *param);

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

int createServerSocketReadyToAccept(int serverSocketfd, int yes,
		struct sigaction* sa, struct addrinfo* p, struct addrinfo* servinfo) {
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((serverSocketfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(serverSocketfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
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
	sa->sa_handler = sigchld_handler;
	sigemptyset(&sa->sa_mask);
	sa->sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	return serverSocketfd;
}

int main(int argc, char **argv) {

	int serverSocketfd, newSock_fd;

	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information

	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;

	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}


	serverSocketfd = createServerSocketReadyToAccept(serverSocketfd, yes, &sa,
			p, servinfo);
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
		threadsActive ++;
		connectionResponse(newSock_fd);


	}

	return 0;
}


void *connectionResponse(int *clientSock_fd) {

	//printf("Llego al connection response \n"); //TODO BORRAR LINEA
	pthread_t thread;

	int iret1 = pthread_create(&thread, NULL, &handshake, clientSock_fd);
	printf("Hilo creado \n"); //TODO BORRAR LINEA
	//Cuarto Parametro:
	//*arg - pointer to argument of function.
	//To pass multiple arguments, send a pointer to a structure.

	if (iret1) {
		// TODO LOGUEAR ERROR
		// TODO Analizar el tratamiento que desea darse
		printf(stderr, "Error - pthread_create() return code: %d\n", iret1);
		exit(1);
	}

	//pthread_join(thread, NULL);

	//exit(EXIT_SUCCESS); //TODO exit_success??

}

void *handshake(void *param) {
	//printf("llego al handshake %d", threadsActive); //TODO borrar linea
	int threadNumber = threadsActive;
	while (1){
		printf("thread  %d \n", threadNumber); //TODO borrar linea
		sleep(1);
	}
}

