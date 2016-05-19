/*
 * protocoloCPU.c
 *
 *  Created on: 19/5/2016
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
#include "protocoloCPU.h"


t_stream *serializar_mensaje(int tipo,void* unaEstructura) {

	t_stream *stream;

	switch(tipo){
	case (2):
			stream = serializar_pedido_bytes_de_una_pagina_a_UMC((t_solicitar_bytes_de_una_pagina_a_UMC *)unaEstructura);
			break;

	case (10):
			stream = serializar_escribir_bytes_de_una_pagina_en_UMC((t_escribir_bytes_de_una_pagina_en_UMC *)unaEstructura);
			break;
	}

	return stream;
}

t_stream *serializar_escribir_bytes_de_una_pagina_en_UMC(t_escribir_bytes_de_una_pagina_en_UMC *escritura){

	uint32_t 	tmpsize = 0,
				offset = 0;

	size_t size_bytes_a_escribir = strlen(escritura->buffer)+1;

	uint32_t 	size_escritura = 	sizeof(uint32_t) +	    //Tamano del numero de pagina
									sizeof(uint32_t) +	    //Tamano del offset
									sizeof(uint32_t) +	    //Tamano del campo datos a escribir
									size_bytes_a_escribir ;	//Tamano del char* de bytes

	uint32_t 	streamSize =	sizeof(uint8_t)	+	//Tamano del tipo
								sizeof(uint32_t)+	//Tamano del largo del stream
								size_escritura;		//Tamano del pedido de pagina

	t_stream *stream = malloc(streamSize);

	memset(stream, 0,streamSize);
	stream->size = size_escritura;
	stream->datos = malloc(streamSize);
	memset(stream->datos,0,streamSize);

	uint8_t tipo = 10; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t numero_pagina = escritura->pagina;
	uint32_t offset_pagina = escritura->offset;
	uint32_t size_pagina = escritura->size;

	char* aux = malloc(streamSize);
	char *escritura_aux = escritura->buffer;


	memcpy(aux,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(aux+offset,&size_escritura,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(aux+offset,&numero_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(aux+offset,&offset_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(aux+offset,&size_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(aux+offset,escritura_aux,tmpsize=size_bytes_a_escribir);
	offset+=tmpsize;

	char endString='\0';
	memcpy(aux+offset,&endString,1);

	stream->datos = aux;

	return stream;



}

t_stream *serializar_pedido_bytes_de_una_pagina_a_UMC(t_solicitar_bytes_de_una_pagina_a_UMC *pedido){
	uint32_t 	tmpsize = 0,
				offset = 0;

	uint32_t 	size_pedido = 	sizeof(uint32_t) +	//Tamano del numero de pagina
								sizeof(uint32_t) +	//Tamano del offset
								sizeof(uint32_t);	//Tamano del campo datos a leer

	uint32_t 	streamSize =	sizeof(uint8_t)	+	//Tamano del tipo
								sizeof(uint32_t)+	//Tamano del largo del stream
								size_pedido;		//Tamano del pedido de pagina

	t_stream *stream = malloc(streamSize);

	memset(stream, 0,streamSize);
	stream->size = size_pedido;
	stream->datos = malloc(streamSize);
	memset(stream->datos,0,streamSize);

	uint8_t tipo = 2; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t numero_pagina = pedido->pagina;
	uint32_t offset_pagina = pedido->offset;
	uint32_t size_pagina = pedido->size;

	memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&size_pedido,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&numero_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&offset_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&size_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	return stream;
}


void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case (3):
			estructuraDestino = deserializar_respuesta_bytes_de_una_pagina_a_CPU (datos);
			break;
	}

	return estructuraDestino;
}

t_respuesta_bytes_de_una_pagina_a_CPU *deserializar_respuesta_bytes_de_una_pagina_a_CPU(char *datos){

	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_respuesta_bytes_de_una_pagina_a_CPU *respuesta = malloc(sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));
	memset(respuesta,0, sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));

	for(tamanoDato = 0; (datos+desplazamientoHeader)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	respuesta->bytes_de_una_pagina = malloc(tamanoDato+1);
	memset(respuesta->bytes_de_una_pagina,0,tamanoDato+1);

	memcpy(respuesta->bytes_de_una_pagina, datos+desplazamientoHeader, tmpsize=tamanoDato+1);
	offset+=tamanoDato+1;
	offset+=desplazamientoHeader;

	char endString = '\0';
	memcpy(respuesta->bytes_de_una_pagina+offset,&endString,1);

	return respuesta;
}
