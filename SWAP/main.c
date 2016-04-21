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

#define PORT "25005"
#define SERVERIP "192.168.1.35"

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
	int sockfd;

	struct addrinfo hints, *servinfo, *p;
	int rv;


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(SERVERIP, PORT, &hints, &servinfo)) != 0) {
		printf("tex");
		exit(1);
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				perror("server: socket");
				continue;
		}


	/*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("client: socket");
		printf("texto");
	return 1;
	*/
	}

	if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		close(sockfd);
		perror("client newton: connect");
		return 1;
	}

	//if (p == NULL) {
	//	fprintf(stderr, "client: failed to connect\n");
	//	return 2;
//	}



	freeaddrinfo(servinfo); // all done with this structure

	printf("client: CONECTADO \n");

	close(sockfd);

	return 0;
}
