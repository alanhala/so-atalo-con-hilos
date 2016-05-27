#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

int create_server_socket_descriptor(char* ip, char *port, int backlog) {
	int yes = 1;
	struct addrinfo sample, *server_info, *p;
	int status, socket_descriptor;

	memset(&sample, 0, sizeof sample);
	sample.ai_family = AF_UNSPEC;
	sample.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(ip, port, &sample, &server_info)) != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(status));
		exit(1);
	}
	for(p = server_info; p != NULL; p = p->ai_next) {
		if ((socket_descriptor = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(socket_descriptor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_descriptor);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(server_info); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}


	printf("Creating socket \n");

	socket_descriptor = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if (socket_descriptor == -1){
		perror("socket error");
		exit(1);
	}

	if (setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	printf("Binding... \n");

	if (bind(socket_descriptor, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		close(socket_descriptor);
		perror("server: bind \n");
		exit(1);
	}

	printf("Starting listening \n");

	if (listen(socket_descriptor, backlog) == -1) {
		perror("listen");
		exit(1);
	}
	return socket_descriptor;
}

int accept_connection(int socket_descriptor) {
	int new_sd;
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr);
	new_sd = accept(socket_descriptor, (struct sockaddr *)&their_addr, &addr_size);
	if (new_sd == -1) {
		perror("accept");
		exit(1);
	}else {
		printf("Connected \n");
		return new_sd;
	}
}

int create_client_socket_descriptor(char *ip, char *port) {
	struct addrinfo sample, *server_info, *p;
	int status, socket_descriptor;

	memset(&sample, 0, sizeof sample);
	sample.ai_family = AF_UNSPEC;
	sample.ai_socktype = SOCK_STREAM;
	if ((status = getaddrinfo(ip, port, &sample, &server_info)) != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(status));
		exit(1);
	}

	for(p = server_info; p != NULL; p = p->ai_next) {
		if ((socket_descriptor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(socket_descriptor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_descriptor);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(2);
	}

	freeaddrinfo(server_info);

	return socket_descriptor;
}
