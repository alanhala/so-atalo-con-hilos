/*
 * protocoloKernel.c
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include "socket.h"
#include "protocoloKernel.h"

t_stream *serializar_mensaje(int tipo, void* unaEstructura) {

	t_stream *stream;



	return stream;
}


void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	}

	return estructuraDestino;
}
