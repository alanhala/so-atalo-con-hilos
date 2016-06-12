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
#include "kernel.h"


//Serializacion
t_stream *serializar_mensaje(int tipo, void* unaEstructura) {

	t_stream *stream;

	switch(tipo){
	case(61):
			stream = serializar_inicio_de_programa_en_UMC((t_inicio_de_programa_en_UMC *)unaEstructura);
	        break;
	case(63):
			stream = serializar_finalizar_programa_en_UMC((t_finalizar_programa_en_UMC *)unaEstructura);
			break;
	case(92):
	         stream = serializar_respuesta_inicio_de_programa_en_kernel((t_respuesta_iniciar_programa_en_kernel *)unaEstructura);
	         break;
	case(121):
			 stream = serializar_enviar_PCB_a_CPU((t_PCB *)unaEstructura);
			 break;
	       }

	return stream;
}

t_stream *serializar_inicio_de_programa_en_UMC(t_inicio_de_programa_en_UMC *inicio_de_programa) {

	int	tmpsize = 0,
		offset = 0;

	size_t codigo_del_programa = strlen(inicio_de_programa->codigo_de_programa)+1;

	uint32_t size_inicio_de_programa =	sizeof(uint32_t) +
										sizeof(uint32_t) +
										codigo_del_programa;

	uint32_t stream_size =  sizeof(uint8_t) +               //Tamano del tipo
							sizeof(uint32_t)+              //Tamano del largo del stream
							size_inicio_de_programa;//Tamano del struct inicio del programa

	t_stream *stream = malloc(sizeof(t_stream));
    memset(stream, 0,sizeof(t_stream));

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

    memcpy(stream->datos+offset,inicio_de_programa->codigo_de_programa,tmpsize=codigo_del_programa);
    offset+=tmpsize;

    char endString='\0';
    memcpy(stream->datos+offset-1,&endString,1);

    return stream;
}

t_stream *serializar_finalizar_programa_en_UMC(t_finalizar_programa_en_UMC *finalizar_programa) {

	uint32_t 	tmpsize=0,
				offset=0;

	uint32_t 	size_pid 	= 	sizeof(uint32_t);

	uint32_t 	stream_size	= 	sizeof(uint8_t) +	//Tamano del tipo
								sizeof(uint32_t)+	//Tamano del total del mensaje
								size_pid;			//Tamano del numero de pid

	t_stream *stream = malloc(sizeof(t_stream));
	memset(stream,0,sizeof(t_stream));

	stream->size = stream_size;
	stream->datos = malloc(stream_size);
	memset(stream->datos,0,stream_size);

	uint8_t tipo = 63;	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t pid = finalizar_programa->process_id;

	memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stream_size,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&pid,sizeof(uint32_t));

	return stream;
}

t_stream *serializar_respuesta_inicio_de_programa_en_kernel(t_respuesta_iniciar_programa_en_kernel *respuesta){

       int tmpsize = 0,
		   offset = 0;

       uint32_t size_respuesta_inicio_programa_en_kernel =	sizeof(uint32_t);

       uint32_t stream_size =  sizeof(uint8_t) +
							   sizeof(uint32_t)+
							   size_respuesta_inicio_programa_en_kernel;

       t_stream *stream = malloc(sizeof(t_stream));
       memset(stream, 0,sizeof(t_stream));

       stream->size = stream_size;
       stream->datos = malloc(stream_size);
       memset(stream->datos,0,stream_size);

       uint8_t tipo = 92; //Tipo del Mensaje . Fijado estaticamente segun protocolo
       uint32_t respuesta_iniciar_programa_en_kernel = respuesta->respuesta_correcta;

       memcpy(stream->datos,&tipo,tmpsize = sizeof(uint8_t));
       offset+=tmpsize;

       memcpy(stream->datos+offset,&stream_size,tmpsize = sizeof(uint32_t));
       offset+=tmpsize;

       memcpy(stream->datos+offset,&respuesta_iniciar_programa_en_kernel,sizeof(uint32_t));

       return stream;
}

t_stream *serializar_enviar_PCB_a_CPU(t_PCB *unPCB){

	uint32_t	tmpsize = 0,
				offset = 0;

	//uint32_t cantidad_elementos_stack = obtiene_cantidad_elementos_stack(unPCB->stack_index);
	//uint32_t sizeof_stack = cantidad_elementos_stack * sizeof(int);

	uint32_t sizeof_instruccion = unPCB->instructions_size * obtiene_sizeof_instrucciones(unPCB->instructions_index);

	uint32_t sizePCB =	sizeof(uint32_t) +
						sizeof(uint32_t) +
						//sizeof_stack	 +
						sizeof(uint32_t) +
						sizeof(uint32_t) +
						sizeof(uint32_t) +
						sizeof(uint32_t) +
						sizeof_instruccion;
						//Lista t_instructions

	uint32_t stream_size = 	sizeof(uint8_t) +	//Tamano del tipo
							sizeof(uint32_t)+	//Tamano del length del mensaje
							//sizeof(uint32_t)+	//Tamano para la cantidad de elementos del stack
							sizePCB;

	t_stream *stream = malloc(sizeof(t_stream));
	memset(stream,0,sizeof(t_stream));

	stream->size = stream_size;

	stream->datos = malloc(stream_size);
	memset(stream->datos,0,stream_size);

	uint8_t		tipo = 121;

	uint32_t 	pid = unPCB->pid,
				program_counter = unPCB->program_counter,
				stack_pointer = unPCB->stack_pointer,
				stack_size = unPCB->stack_size,
				used_pages = unPCB->used_pages,
				instructions_size = unPCB->instructions_size;

	memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stream_size,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	//memcpy(stream->datos+offset,&cantidad_elementos_stack,tmpsize=sizeof(uint32_t));
	//offset+=tmpsize;

	memcpy(stream->datos+offset,&pid,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&program_counter,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	/*
	int elementos_del_stack_recorridos = 0;
	int elementos_del_stack[cantidad_elementos_stack];

	obtiene_elementos_del_stack(unPCB->stack_index,elementos_del_stack);

	while (elementos_del_stack_recorridos<cantidad_elementos_stack){

		memcpy(stream->datos+offset,&elementos_del_stack[elementos_del_stack_recorridos],tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		elementos_del_stack_recorridos++;
	}
	*/

	memcpy(stream->datos+offset,&stack_pointer,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stack_size,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&used_pages,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&instructions_size,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	int contador_de_instrucciones = 0;

	while (contador_de_instrucciones<instructions_size){

	t_puntero_instruccion primera_instruccion = obtiene_primera_instruccion((unPCB->instructions_index)[contador_de_instrucciones]);
	memcpy(stream->datos+offset,&primera_instruccion,tmpsize=sizeof(t_puntero_instruccion));
	offset+=tmpsize;

	t_size offset_instruccion = obtiene_offset((unPCB->instructions_index)[contador_de_instrucciones]);
	memcpy(stream->datos+offset,&offset_instruccion,tmpsize=sizeof(t_size));
	offset+=tmpsize;

	contador_de_instrucciones++;
	}

	return stream;
}


//Deserealizacion
void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	 case(62):
		   estructuraDestino = deserealizar_respuesta_iniciar_programa_en_UMC(datos);
		   break;
	 case (64):
		   estructuraDestino = deserealizar_respuesta_finalizar_programa_en_UMC(datos);
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

t_respuesta_finalizar_programa_en_UMC *deserealizar_respuesta_finalizar_programa_en_UMC(char *datos){

	const int desplazamiento_header = 5;	//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_respuesta_finalizar_programa_en_UMC *respuesta = malloc(sizeof(t_respuesta_finalizar_programa_en_UMC));
	memset(respuesta,0,sizeof(t_respuesta_finalizar_programa_en_UMC));

	memcpy(&respuesta->respuesta_correcta,datos+desplazamiento_header,sizeof(uint32_t));

	return respuesta;
}

t_iniciar_programa_en_kernel *deserealizar_iniciar_programa_en_kernel (char *datos){

	int	tamano_dato = 0;

	const int desplazamiento_header = 5;    //Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

    t_iniciar_programa_en_kernel *iniciar_programa_en_kernel = malloc(sizeof(t_iniciar_programa_en_kernel));
    memset(iniciar_programa_en_kernel,0,sizeof(t_iniciar_programa_en_kernel));

    for(tamano_dato = 0; (datos+desplazamiento_header)[tamano_dato] != '\0';tamano_dato++);//incremento tamanoDato, hasta el tamaÃ±o del nomb

    iniciar_programa_en_kernel->codigo_de_programa = malloc(tamano_dato+1);
    memset(iniciar_programa_en_kernel->codigo_de_programa,0,tamano_dato+1);

    memcpy(iniciar_programa_en_kernel->codigo_de_programa,datos+desplazamiento_header,(tamano_dato+1));

    char endString = '\0';
    memcpy(iniciar_programa_en_kernel->codigo_de_programa+tamano_dato,&endString,1);

    return iniciar_programa_en_kernel;
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



//Funciones Auxiliares

uint32_t obtiene_sizeof_instrucciones(t_intructions *instrucciones){

	uint32_t sizeof_puntero_primera_instruccion = sizeof(instrucciones->start);
	uint32_t sizeof_offset = sizeof(instrucciones->offset);

	return (sizeof_puntero_primera_instruccion+sizeof_offset);
}

/*
uint32_t obtiene_cantidad_elementos_stack(t_list *stack_index){

	uint32_t cuenta_elementos_de_la_lista = 0;	//Lo empiezo en 1 por como aumento el contador

	t_list *aux = stack_index;	//Recorro la lista con un puntero auxiliar para no desreferenciar la lista

	while(aux!=NULL){
		cuenta_elementos_de_la_lista++;
		aux = aux->head;	//Avanzo el puntero a t_list
	}

	return cuenta_elementos_de_la_lista;
}

void obtiene_elementos_del_stack(t_list *stack_index, int elementos_del_stack[]){

	t_list *aux = stack_index;	//Recorro la lista con un puntero auxiliar para no desreferenciar la lista

	int i = 0;

	while (aux!=NULL){

		elementos_del_stack[i] = aux->elements_count;

		i++;

		aux = aux->head;	//Avanzo el puntero a t_list
	}
}
*/

t_puntero_instruccion obtiene_primera_instruccion(t_intructions instruccion){

	t_puntero_instruccion una_instruccion = instruccion.start;

	return una_instruccion;
}

t_size obtiene_offset (t_intructions instruccion){

	t_size offset = instruccion.offset;

	return offset;
}
