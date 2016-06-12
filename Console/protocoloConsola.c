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

//Serializacion
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

	uint32_t stream_size =	sizeof(uint8_t) +			//Tamano del tipo
							sizeof(uint32_t)+			//Tamano del largo del stream
							size_codigo_del_programa;	//Tamano del codigo

	t_stream *stream = malloc(sizeof(t_stream));
	memset(stream, 0,sizeof(t_stream));

	stream->size = stream_size;
	stream->datos = malloc(stream_size);
	memset(stream->datos,0,stream_size);

	uint8_t tipo = 91; //Tipo del Mensaje . Fijado estaticamente segun protocolo

	memcpy(stream->datos,&tipo,tmpsize = sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stream_size,tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,inicio_programa->codigo_de_programa,tmpsize=size_codigo_del_programa);
	offset+=tmpsize;

	char endString='\0';
	memcpy(stream->datos+offset-1,&endString,1);

	return stream;
}

void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case(92):
			estructuraDestino = deserealizar_respuesta_inicio_de_programa_en_kernel(datos);
			break;
	case(132):
			estructuraDestino = deserealizar_imprimir_texto_en_consola(datos);
			break;
	}

	return estructuraDestino;
}

t_imprimir_texto_en_consola *  deserealizar_imprimir_texto_en_consola(char *datos){

	int		tmpsize = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_imprimir_texto_en_consola *texto_a_imprimir = malloc(sizeof(t_imprimir_texto_en_consola));
	memset(texto_a_imprimir,0, sizeof(t_imprimir_texto_en_consola));

	for(tamanoDato = 0; (datos+desplazamientoHeader)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	texto_a_imprimir->texto_a_imprimir = malloc(tamanoDato+1);
	memset(texto_a_imprimir->texto_a_imprimir,0,tamanoDato+1);

	memcpy(texto_a_imprimir->texto_a_imprimir, datos+desplazamientoHeader, tmpsize=tamanoDato+1);

	char endString = '\0';
	memcpy(texto_a_imprimir->texto_a_imprimir+tamanoDato,&endString,1);

	return texto_a_imprimir;

}

t_respuesta_iniciar_programa_en_kernel *deserealizar_respuesta_inicio_de_programa_en_kernel(char *datos){

	const int desplazamiento_header = 5;	//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_respuesta_iniciar_programa_en_kernel *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_kernel));
	memset(respuesta,0,sizeof(t_respuesta_iniciar_programa_en_kernel));

	memcpy(&respuesta->respuesta_correcta,datos+desplazamiento_header,sizeof(uint32_t));

	return respuesta;
}

t_header *deserializar_header(char *header){

	uint32_t	tmpsize=0,
				offset=0;

	t_header *un_header = malloc(sizeof(t_header));

	memcpy(&un_header->tipo,header,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(&un_header->length,header+offset,sizeof(uint32_t));

	return un_header;
}

