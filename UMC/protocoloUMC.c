/*
 * protocoloUMC.c
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
#include "protocoloUMC.h"



void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case (2):
			estructuraDestino = deserializar_pedido_bytes_de_una_pagina_a_UMC(datos);

			break;
	case (10):
			estructuraDestino = deserializar_escribir_bytes_de_una_pagina_en_UMC(datos);
			break;

	}

	return estructuraDestino;
}

t_escribir_bytes_de_una_pagina_en_UMC *deserializar_escribir_bytes_de_una_pagina_en_UMC(char *datos){
	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_escribir_bytes_de_una_pagina_en_UMC *escritura = malloc(sizeof(t_escribir_bytes_de_una_pagina_en_UMC));
	memset(escritura,0, sizeof(t_escribir_bytes_de_una_pagina_en_UMC));


	memcpy(&escritura->pagina, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&escritura->offset, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&escritura->size, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;


	for(tamanoDato = 0; (datos+offset)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	escritura->buffer = malloc(tamanoDato+1);
	memset(escritura->buffer,0,tamanoDato+1);

	memcpy(escritura->buffer, datos+offset, tmpsize=tamanoDato+1);
	offset+=tamanoDato+1;
	offset+=desplazamientoHeader;

	char endString = '\0';
	memcpy(escritura->buffer+offset,&endString,1);


	return escritura;


}


t_solicitar_bytes_de_una_pagina_a_UMC *deserializar_pedido_bytes_de_una_pagina_a_UMC(char *datos){

	int		tmpsize = 0,
			offset = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_solicitar_bytes_de_una_pagina_a_UMC *pedido = malloc(sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));
	memset(pedido,0, sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));

	memcpy(&pedido->pagina, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&pedido->offset, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&pedido->size, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	return pedido;
}

t_stream *serializar_mensaje(int tipo, void* unaEstructura) {

	t_stream *stream;

	switch(tipo){
	case (3):
			stream = serializar_respuesta_bytes_de_una_pagina_a_CPU((t_respuesta_bytes_de_una_pagina_a_CPU *)unaEstructura);
			break;
	}

	return stream;
}


t_stream *serializar_respuesta_bytes_de_una_pagina_a_CPU(t_respuesta_bytes_de_una_pagina_a_CPU *unaEstructura){

	uint32_t 	tmpsize = 0,
				offset = 0;

	size_t size_bytes_de_una_pagina = strlen(unaEstructura->bytes_de_una_pagina)+1;

	uint32_t streamSize =	sizeof(uint8_t)  +			//Tamano del tipo
							sizeof(uint32_t) +			//Tamano del largo del stream
							size_bytes_de_una_pagina ;	//Tamano del char* de bytes

	t_stream *stream = malloc(streamSize);

	memset(stream, 0,streamSize+1);
	stream->size = streamSize;
	stream->datos = malloc(streamSize);
	memset(stream->datos,0,streamSize);


 	char* aux = malloc(streamSize);
 	char *respuesta_aux = unaEstructura->bytes_de_una_pagina;

	uint8_t tipo = 3; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo

	memcpy(aux,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(aux+offset,&size_bytes_de_una_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(aux+offset,respuesta_aux,tmpsize=size_bytes_de_una_pagina);
	offset+=tmpsize;

	char endString='\0';
	memcpy(aux+offset,&endString,1);

	stream->datos = aux;

	return stream;
}
