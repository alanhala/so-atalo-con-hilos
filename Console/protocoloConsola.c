/*
 * protocoloConsola.c
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
#include "protocoloConsola.h"

t_stream *serializar_mensaje(int tipo, void* unaEstructura) {

	t_stream *stream;

	switch(tipo){
	case(91):
			stream = serializar_iniciar_programa_en_kernel((t_iniciar_programa_en_kernel *)unaEstructura);
			break;
	}

	return stream;
}

t_stream *serializar_iniciar_programa_en_kernel(t_iniciar_programa_en_kernel *inicio_programa){

	int tmpsize = 0,
		offset = 0;

	size_t size_codigo_del_programa = strlen(inicio_programa->codigo_de_programa)+1;

	uint32_t stream_size = 		sizeof(uint8_t) +			//Tamano del tipo
								sizeof(uint32_t) +			//Tamano del largo del stream
								size_codigo_del_programa;	//Tamano del codigo

	t_stream *stream = malloc(sizeof(stream_size));
	memset(stream, 0,stream_size);
	stream->size = stream_size;
	stream->datos = malloc(stream_size);
	memset(stream->datos,0,stream_size);

	uint8_t tipo = 91; //Tipo del Mensaje . Fijado estaticamente segun protocolo

	memcpy(stream->datos,&tipo,tmpsize = sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stream_size,tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,inicio_programa->codigo_de_programa,size_codigo_del_programa);
	offset+=tmpsize;

	char endString='\0';
	memcpy(stream->datos+size_codigo_del_programa,&endString,1);

	return stream;
}
void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case(92):
			estructuraDestino = deserealizar_respuesta_inicio_de_programa_en_kernel(datos);
			break;
	}

	return estructuraDestino;
}

t_respuesta_iniciar_programa_en_kernel *deserealizar_respuesta_inicio_de_programa_en_kernel(char *datos){

	int tmpsize = 0;

	const int desplazamiento_header = 5;	//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_respuesta_iniciar_programa_en_kernel *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_kernel));
	memset(respuesta,0,sizeof(t_respuesta_iniciar_programa_en_kernel));

	memcpy(&respuesta->respuesta_correcta,datos+desplazamiento_header,tmpsize = sizeof(uint32_t));

	return respuesta;
}
