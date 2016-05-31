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
			estructuraDestino = deserializar_respuesta_iniciar_programa_en_swap(datos);
			break;
	case (4):
			estructuraDestino = deserializar_respuesta_leer_pagina_swap(datos);
		    break;
	case (6):
			estructuraDestino = deserializar_respuesta_escribir_pagina_swap(datos);
	        break;
	case (8):
			estructuraDestino = deserializar_pedido_bytes_de_una_pagina_a_UMC(datos);
			break;
	case (10):
			estructuraDestino = deserializar_escribir_bytes_de_una_pagina_en_UMC(datos);
			break;

	}

	return estructuraDestino;
}

t_respuesta_escribir_pagina_swap * deserializar_respuesta_escribir_pagina_swap(char *datos){
			int		tmpsize = 0,
					offset = 0;

			const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

			t_respuesta_escribir_pagina_swap *respuesta = malloc(sizeof(t_respuesta_escribir_pagina_swap));
			memset(respuesta,0, sizeof(t_respuesta_escribir_pagina_swap));

			memcpy(&respuesta->escritura_correcta, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
			offset+=tmpsize;
			offset+=desplazamientoHeader;


			return respuesta;

}

t_respuesta_leer_pagina_swap *deserializar_respuesta_leer_pagina_swap(char *datos){

	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_respuesta_leer_pagina_swap *respuesta = malloc(sizeof(t_respuesta_leer_pagina_swap));
	memset(respuesta,0, sizeof(t_respuesta_leer_pagina_swap));

	for(tamanoDato = 0; (datos+desplazamientoHeader)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	respuesta->datos = malloc(tamanoDato+1);
	memset(respuesta->datos,0,tamanoDato+1);

	memcpy(respuesta->datos, datos+desplazamientoHeader, tmpsize=tamanoDato+1);
	offset+=tamanoDato+1;
	offset+=desplazamientoHeader;

	char endString = '\0';
	memcpy(respuesta->datos+offset,&endString,1);

	return respuesta;

}


t_respuesta_iniciar_programa_en_swap * deserializar_respuesta_iniciar_programa_en_swap(char* datos){
		int		tmpsize = 0,
				offset = 0;

		const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

		t_respuesta_iniciar_programa_en_swap *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_swap));
		memset(respuesta,0, sizeof(t_respuesta_iniciar_programa_en_swap));

		memcpy(&respuesta->cargado_correctamente, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
		offset+=tmpsize;
		offset+=desplazamientoHeader;


		return respuesta;

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
	case (1):
				stream = serializar_iniciar_programa_en_swap((t_iniciar_programa_en_swap *)unaEstructura);
				break;
	case (3):
			    stream = serializar_leer_pagina_swap((t_leer_pagina_swap *)unaEstructura);
			    break;
	case (5):
				stream = serializar_escribir_pagina_swap((t_escribir_pagina_swap *)unaEstructura);
				break;
	case (7):
			stream = serializar_respuesta_bytes_de_una_pagina_a_CPU((t_respuesta_bytes_de_una_pagina_a_CPU *)unaEstructura);
			break;
	case (9):
			stream = serializar_respuesta_escribir_bytes_de_una_pagina_en_UMC((t_respuesta_escribir_bytes_de_una_pagina_en_UMC *)unaEstructura);
			break;
	  }

	return stream;
}
t_stream * serializar_escribir_pagina_swap(t_escribir_pagina_swap * escritura){

		uint32_t 	tmpsize = 0,
	    			offset = 0;

	    size_t size_datos = strlen(escritura->datos)+1;

	    uint32_t 	size_escritura = 	sizeof(uint32_t) +	    //Tamano del pid
										sizeof(uint32_t) +	    //Tamano de pagina
										size_datos ;			//Tamano del char* de bytes

	    uint32_t 	streamSize =		sizeof(uint8_t)	+	//Tamano del tipo
	                  	  	  	  		sizeof(uint32_t)+					//Tamano del largo del stream
										size_escritura;					//Tamano del pedido

	    t_stream *stream = malloc(streamSize);

	    memset(stream, 0,streamSize);
	    stream->size = streamSize;
	    stream->datos = malloc(streamSize);
	    memset(stream->datos,0,streamSize);

	    uint8_t tipo = 26; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	    uint32_t pid = escritura->pid;
	    uint32_t pagina = escritura->pagina;


	    char* aux = malloc(streamSize);
	    char *escritura_aux = escritura->datos;


	    memcpy(aux,&tipo,tmpsize=sizeof(uint8_t));
	    offset+=tmpsize;

	    memcpy(aux+offset,&size_escritura,tmpsize=sizeof(uint32_t));
	    offset+=tmpsize;

	    memcpy(aux+offset,&pid,tmpsize=sizeof(uint32_t));
	    offset+=tmpsize;

	    memcpy(aux+offset,&pagina,tmpsize=sizeof(uint32_t));
	    offset+=tmpsize;

	    memcpy(aux+offset,escritura_aux,tmpsize=size_datos);
	    offset+=tmpsize;

	    char endString='\0';
	    memcpy(aux+offset,&endString,1);

	    stream->datos = aux;

	    return stream;

}

t_stream * serializar_leer_pagina_swap(t_leer_pagina_swap * pedido){
		uint32_t 	tmpsize = 0,
	    			offset = 0;

		uint32_t 	size_pedido = 	sizeof(uint32_t) +	    //Tamano del pid
									sizeof(uint32_t) ;   	//Tamano de pagina

	    uint32_t 	streamSize =	sizeof(uint8_t)	+	//Tamano del tipo
									sizeof(uint32_t)+	//Tamano del largo del stream
									size_pedido;		//Tamano del pedido

	    t_stream *stream = malloc(streamSize);

	    memset(stream, 0,streamSize);
	    stream->size = streamSize;
	    stream->datos = malloc(streamSize);
	    memset(stream->datos,0,streamSize);

	    uint8_t tipo = 22; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	    uint32_t pid = pedido->pid;
	    uint32_t pagina = pedido->pagina;


		memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&size_pedido,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&pid,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&pagina,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;


		return stream;

}

t_stream * serializar_iniciar_programa_en_swap(t_iniciar_programa_en_swap * pedido){

    uint32_t 	tmpsize = 0,
    			offset = 0;

    size_t size_codigo = strlen(pedido->codigo_programa)+1;

    uint32_t 	size_escritura = 	sizeof(uint32_t) +	    //Tamano del pid
                    				sizeof(uint32_t) +	    //Tamano de paginas necesarias
									size_codigo ;			//Tamano del char* de bytes

    uint32_t 	streamSize =	sizeof(uint8_t)	+	//Tamano del tipo
                  	  	  	  	sizeof(uint32_t)+	//Tamano del largo del stream
								size_escritura;		//Tamano del pedido

    t_stream *stream = malloc(streamSize);

    memset(stream, 0,streamSize);
    stream->size = streamSize;
    stream->datos = malloc(streamSize);
    memset(stream->datos,0,streamSize);

    uint8_t tipo = 20; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
    uint32_t pid = pedido->pid;
    uint32_t paginas_necesarias = pedido->paginas_necesarias;


    char* aux = malloc(streamSize);
    char *pedido_aux = pedido->codigo_programa;


    memcpy(aux,&tipo,tmpsize=sizeof(uint8_t));
    offset+=tmpsize;

    memcpy(aux+offset,&size_escritura,tmpsize=sizeof(uint32_t));
    offset+=tmpsize;

    memcpy(aux+offset,&pid,tmpsize=sizeof(uint32_t));
    offset+=tmpsize;

    memcpy(aux+offset,&paginas_necesarias,tmpsize=sizeof(uint32_t));
    offset+=tmpsize;

    memcpy(aux+offset,pedido_aux,tmpsize=size_codigo);
    offset+=tmpsize;

    char endString='\0';
    memcpy(aux+offset,&endString,1);

    stream->datos = aux;

    return stream;

}

t_stream *serializar_respuesta_escribir_bytes_de_una_pagina_en_UMC(t_respuesta_escribir_bytes_de_una_pagina_en_UMC *respuesta){
		uint32_t 	tmpsize = 0,
					offset = 0;

		uint32_t 	size_respuesta = sizeof(uint32_t);	//Tamano de la respuesta

		uint32_t 	streamSize =	sizeof(uint8_t)	+	//Tamano del tipo
									sizeof(uint32_t)+	//Tamano del largo del stream
									size_respuesta;		//Tamano de la respuesta

		t_stream *stream = malloc(streamSize);

		memset(stream, 0,streamSize);
		stream->size = size_respuesta;
		stream->datos = malloc(streamSize);
		memset(stream->datos,0,streamSize);


		uint8_t tipo = 2; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
		uint32_t escritura_resultado = respuesta->escritura_correcta;

		memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&size_respuesta,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&escritura_resultado,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

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
