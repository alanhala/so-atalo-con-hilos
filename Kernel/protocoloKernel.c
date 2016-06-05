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
	                       break;
	       case(92):
	                       stream = serializar_respuesta_inicio_de_programa_en_kernel((t_respuesta_iniciar_programa_en_kernel *)unaEstructura);
	                       break;
	       }

	return stream;
}

t_stream *serializar_inicio_de_programa_en_UMC(t_inicio_de_programa_en_UMC *inicio_de_programa) {

       int tmpsize = 0,
               offset = 0;

       size_t codigo_del_programa = strlen(inicio_de_programa->codigo_de_programa)+1;

       uint32_t size_inicio_de_programa =      sizeof(uint32_t) +
											   sizeof(uint32_t) +
											   codigo_del_programa;

       uint32_t stream_size =  sizeof(uint8_t) +               //Tamano del tipo
							   sizeof(uint32_t) +              //Tamano del largo del stream
							   size_inicio_de_programa;//Tamano del struct inicio del programa

       t_stream *stream = malloc(sizeof(stream_size));
       memset(stream, 0,stream_size);
       stream->size = stream_size;
       stream->datos = malloc(stream_size);
       memset(stream->datos,0,stream_size);

       uint8_t tipo = 61;      //Tipo del Mensaje . Fijado estaticamente segun protocolo
       uint32_t process_id = inicio_de_programa->process_id;
       uint32_t cantidad_de_paginas = inicio_de_programa->cantidad_de_paginas;

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
       memcpy(stream->datos+size_inicio_de_programa,&endString,1);

       return stream;

}

t_stream *serializar_respuesta_inicio_de_programa_en_kernel(t_respuesta_iniciar_programa_en_kernel *respuesta){

       int tmpsize = 0,
		   offset = 0;

       uint32_t size_respuesta_inicio_programa_en_kernel =     sizeof(uint32_t);

       uint32_t stream_size =  sizeof(uint8_t) +
							   sizeof(uint32_t) +
							   size_respuesta_inicio_programa_en_kernel;

       t_stream *stream = malloc(sizeof(stream_size));
       memset(stream, 0,stream_size);
       stream->size = stream_size;
       stream->datos = malloc(stream_size);
       memset(stream->datos,0,stream_size);

       uint8_t tipo = 92; //Tipo del Mensaje . Fijado estaticamente segun protocolo
       uint32_t respuesta_iniciar_programa_en_kernel = respuesta->respuesta_correcta;

       memcpy(stream->datos,&tipo,tmpsize = sizeof(uint8_t));
       offset+=tmpsize;

       memcpy(stream->datos+offset,&stream_size,tmpsize = sizeof(uint32_t));
       offset+=tmpsize;

       memcpy(stream->datos+offset,&respuesta_iniciar_programa_en_kernel,tmpsize = sizeof(uint32_t));
       offset+=tmpsize;

       return stream;
}


void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	 case(62):
		   estructuraDestino = deserealizar_respuesta_iniciar_programa_en_UMC(datos);
		   break;
	 case (91):
		   estructuraDestino = deserealizar_iniciar_programa_en_kernel(datos);
		   break;

	}

	return estructuraDestino;
}

t_respuesta_iniciar_programa_en_UMC *deserealizar_respuesta_iniciar_programa_en_UMC(char *datos){

       const int desplazamiento_header = 5;    //Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

       t_respuesta_iniciar_programa_en_UMC *respuesta= malloc(sizeof(t_respuesta_iniciar_programa_en_UMC));
       memset(respuesta,0,sizeof(t_respuesta_iniciar_programa_en_UMC));

       memcpy(&respuesta->respuesta_correcta,datos+desplazamiento_header,sizeof(uint32_t));

       return respuesta;
}

t_iniciar_programa_en_kernel *deserealizar_iniciar_programa_en_kernel (char *datos){

       int tmpsize = 0,
               offset = 0,
               tamano_dato = 0;

       const int desplazamiento_header = 5;    //Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

       t_iniciar_programa_en_kernel *iniciar_programa_en_kernel = malloc(sizeof(t_iniciar_programa_en_kernel));
       memset(iniciar_programa_en_kernel,0,sizeof(t_iniciar_programa_en_kernel));

       for(tamano_dato = 0; (datos+desplazamiento_header)[tamano_dato] != '\0';tamano_dato++);//incremento tamanoDato, hasta el tamaÃ±o del nomb

       iniciar_programa_en_kernel->codigo_de_programa = malloc(tamano_dato+1);
       memset(iniciar_programa_en_kernel->codigo_de_programa,0,tamano_dato+1);

       memcpy(iniciar_programa_en_kernel->codigo_de_programa,datos+desplazamiento_header,tmpsize=tamano_dato+1);
       offset+=tamano_dato+1;
       offset+=desplazamiento_header;

       char endString = '\0';
       memcpy(iniciar_programa_en_kernel->codigo_de_programa+tamano_dato+1,&endString,1);

       return iniciar_programa_en_kernel;
}



