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

	switch(tipo){
	case(61):
			stream = serializar_inicio_de_programa_en_UMC((t_inicio_de_programa_en_UMC *)unaEstructura);
	  }

	return stream;
}

t_stream serializar_inicio_de_programa_en_UMC(t_inicio_de_programa_en_UMC inicio_de_programa) {

	int tmpsize = 0,
		offset = 0;

	size_t codigo_del_programa = strlen(inicio_de_programa.codigo_de_programa)+1;

	uint32_t size_inicio_de_programa = 	sizeof(uint32_t) +
										sizeof(uint32_t) +
										codigo_del_programa;

	uint32_t stream_size = 	sizeof(uint8_t) +		//Tamano del tipo
							sizeof(uint32_t) +		//Tamano del largo del stream
							size_inicio_de_programa;//Tamano del struct inicio del programa

	t_stream *stream = malloc(sizeof(stream_size));
	memset(stream, 0,stream_size);
	stream->size = stream_size;
	stream->datos = malloc(stream_size);
	memset(stream->datos,0,stream_size);

	uint8_t tipo = 61;	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t process_id = inicio_de_programa.process_id;
	uint32_t cantidad_de_paginas = inicio_de_programa.cantidad_de_paginas;

	memcpy(stream->datos,&tipo,tmpsize = sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stream_size,tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&process_id,tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&cantidad_de_paginas,tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,inicio_de_programa->codigo_de_programa,tmpsize = sizeof(size_inicio_de_programa));
	offset+=tmpsize;

	char endString='\0';
	memcpy(stream->datos+offset,&endString,1);

	return stream;

}


void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	}

	return estructuraDestino;
}
