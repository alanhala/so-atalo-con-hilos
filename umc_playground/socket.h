/*
 * socket.h
 *
 *  Created on: 28/4/2016
 *      Author: utnso
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

int create_server_socket_descriptor(char* port, int backlog);
int accept_connection(int socket_descriptor);
int create_client_socket_descriptor(char* ip, char* port);

#endif /* SOCKET_H_ */
