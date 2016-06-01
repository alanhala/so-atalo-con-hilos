/*
 * protocolo_swap.c
 *
 *  Created on: 25/5/2016
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
#include "socket.h"
#include "protocolo_swap.h"



void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case (1):
			estructuraDestino =  deserializar_iniciar_programa_en_swap(datos);
			break;

	case (3):
			estructuraDestino =  deserializar_lectura_pagina_swap(datos);
			break;

	case (5):
			estructuraDestino =  deserializar_escribir_pagina_swap(datos);
			break;

	}

	return estructuraDestino;
}

t_escribir_pagina_swap * deserializar_escribir_pagina_swap(char *datos){

	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_escribir_pagina_swap *escritura = malloc(sizeof(t_escribir_pagina_swap));

	memset(escritura,0, sizeof(t_escribir_pagina_swap));

	memcpy(&escritura->pid, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&escritura->pagina, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	for(tamanoDato = 0; (datos+offset)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	escritura->datos = malloc(tamanoDato+1);
	memset(escritura->datos,0,tamanoDato+1);

	memcpy(escritura->datos, datos+offset, tmpsize=tamanoDato+1);
	offset+=tamanoDato+1;
	offset+=desplazamientoHeader;

	char endString = '\0';
	memcpy(escritura->datos+offset,&endString,1);

	return escritura;

}


t_leer_pagina_swap * deserializar_lectura_pagina_swap(char * datos){

	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_leer_pagina_swap *pedido = malloc(sizeof(t_leer_pagina_swap));
	memset(pedido,0, sizeof(t_leer_pagina_swap));

	memcpy(&pedido->pid, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&pedido->pagina, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	return pedido;
}



t_iniciar_programa_en_swap * deserializar_iniciar_programa_en_swap(char *datos){

	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_iniciar_programa_en_swap *pedido = malloc(sizeof(t_iniciar_programa_en_swap));
	memset(pedido,0, sizeof(t_iniciar_programa_en_swap));

	memcpy(&pedido->pid, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&pedido->paginas_necesarias, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	for(tamanoDato = 0; (datos+offset)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	pedido->codigo_programa = malloc(tamanoDato+1);
	memset(pedido->codigo_programa,0,tamanoDato+1);

	memcpy(pedido->codigo_programa, datos+offset, tmpsize=tamanoDato+1);
	offset+=tamanoDato+1;
	offset+=desplazamientoHeader;

	char endString = '\0';
	memcpy(pedido->codigo_programa+offset,&endString,1);

	return pedido;
}



t_stream *serializar_mensaje(int tipo, void* unaEstructura) {

	t_stream *stream;

	switch(tipo){
	case (2):
			stream = serializar_respuesta_iniciar_programa_en_swap((t_respuesta_iniciar_programa_en_swap*)unaEstructura);
			break;
	case (4):
		    stream = serializar_respuesta_leer_pagina_swap((t_respuesta_leer_pagina_swap *)unaEstructura);
		    break;
	case (6):
			stream = serializar_respuesta_escribir_pagina_swap((t_respuesta_escribir_pagina_swap *)unaEstructura);
	        break;

    }
	return stream;
}


t_stream * serializar_respuesta_leer_pagina_swap(t_respuesta_leer_pagina_swap *respuesta){

		uint32_t 	tmpsize = 0,
					offset = 0;

		size_t size_bytes_de_lectura = strlen(respuesta->datos)+1;

		uint32_t streamSize =	sizeof(uint8_t)  +			//Tamano del tipo
								sizeof(uint32_t) +			//Tamano del largo del stream
								size_bytes_de_lectura ;	//Tamano del char* de bytes

		t_stream *stream = malloc(streamSize);

		memset(stream, 0,streamSize+1);
		stream->size = streamSize;
		stream->datos = malloc(streamSize);
		memset(stream->datos,0,streamSize);

	 	char* aux = malloc(streamSize);
	 	char *respuesta_aux = respuesta->datos;

		uint8_t tipo = 4; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo

		memcpy(aux,&tipo,tmpsize=sizeof(uint8_t));
		offset+=tmpsize;

		memcpy(aux+offset,&streamSize,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(aux+offset,respuesta_aux,tmpsize=size_bytes_de_lectura);
		offset+=tmpsize;

		char endString='\0';
		memcpy(aux+offset,&endString,1);

		stream->datos = aux;

		return stream;
}

t_stream * serializar_respuesta_escribir_pagina_swap(t_respuesta_escribir_pagina_swap *respuesta){
			uint32_t 	tmpsize = 0,
						offset = 0;

			uint32_t 	size_respuesta =	sizeof(uint32_t);    //Tamano de escritura correcta

			uint32_t 	streamSize =	sizeof(uint8_t)	+	//Tamano del tipo
										sizeof(uint32_t)+	//Tamano del largo del stream
										size_respuesta;		//Tamano de la respuesta

			t_stream *stream = malloc(streamSize);

			memset(stream, 0,streamSize);
			stream->size = streamSize;
			stream->datos = malloc(streamSize);
			memset(stream->datos,0,streamSize);

			uint8_t tipo = 6; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
			uint32_t escrito_correctamente = respuesta->escritura_correcta;

			memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
			offset+=tmpsize;

			memcpy(stream->datos+offset,&streamSize,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			memcpy(stream->datos+offset,&escrito_correctamente,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;
			return stream;

}

t_stream * serializar_respuesta_iniciar_programa_en_swap(t_respuesta_iniciar_programa_en_swap* respuesta){
			uint32_t 	tmpsize = 0,
		    			offset = 0;

			uint32_t 	size_respuesta =	sizeof(uint32_t); 	    //Tamano de respuesta cargada

		    uint32_t 	streamSize =	sizeof(uint8_t)	+	//Tamano del tipo
										sizeof(uint32_t)+	//Tamano del largo del stream
										size_respuesta;		//Tamano de la respuesta

		    t_stream *stream = malloc(streamSize);

		    memset(stream, 0,streamSize);
		    stream->size = streamSize;
		    stream->datos = malloc(streamSize);
		    memset(stream->datos,0,streamSize);

		    uint8_t tipo = 2; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
		    uint32_t cargado_correctamente = respuesta->cargado_correctamente;



			memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
			offset+=tmpsize;

			memcpy(stream->datos+offset,&streamSize,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			memcpy(stream->datos+offset,&cargado_correctamente,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;
			return stream;

}
