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


//Serializacion
t_stream *serializar_mensaje(int tipo,void* unaEstructura) {

	t_stream *stream;

	switch(tipo){
	case (31):
			stream = serializar_pedido_bytes_de_una_pagina_a_UMC((t_solicitar_bytes_de_una_pagina_a_UMC *)unaEstructura);
			break;
	case (33):
			stream = serializar_escribir_bytes_de_una_pagina_en_UMC((t_escribir_bytes_de_una_pagina_en_UMC *)unaEstructura);
			break;
	case (35):
			stream = serializar_cambio_de_proceso((t_cambio_de_proceso *)unaEstructura);
			break;
	case (132):
			stream = serializar_imprimir_texto_a_cpu((t_imprimir_texto_en_cpu *)unaEstructura);
			break;
	}

	return stream;
}

t_stream *serializar_imprimir_texto_a_cpu(t_imprimir_texto_en_cpu *unaEstructura){
		uint32_t 	tmpsize = 0,
					offset = 0;

		size_t size_texto_a_imprimir = strlen(unaEstructura->texto_a_imprimir)+1;

		uint32_t stream_size =	sizeof(uint8_t)  +			//Tamano del tipo
								sizeof(uint32_t) +			//Tamano del largo del stream
								size_texto_a_imprimir ;	//Tamano del char* de bytes

		t_stream *stream = malloc(sizeof(t_stream));
		memset(stream, 0,sizeof(t_stream));

		stream->size = stream_size;
		stream->datos = malloc(stream_size);
		memset(stream->datos,0,stream_size);

		uint8_t tipo = 132; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo

		memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&stream_size,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,unaEstructura->texto_a_imprimir,tmpsize=size_texto_a_imprimir);
		offset+=tmpsize;

		char endString='\0';
		memcpy(stream->datos+offset-1,&endString,1);

		return stream;

}

t_stream *serializar_pedido_bytes_de_una_pagina_a_UMC(t_solicitar_bytes_de_una_pagina_a_UMC *pedido){

	uint32_t 	tmpsize = 0,
				offset = 0;

	uint32_t 	size_pedido = 	sizeof(uint32_t) +	//Tamano del numero de pagina
								sizeof(uint32_t) +	//Tamano del offset
								sizeof(uint32_t);	//Tamano del campo datos a leer

	uint32_t 	stream_size =	sizeof(uint8_t)	+	//Tamano del tipo
								sizeof(uint32_t)+	//Tamano del largo del stream
								size_pedido;		//Tamano del pedido de pagina

	t_stream *stream = malloc(sizeof(t_stream));
	memset(stream, 0,sizeof(t_stream));

	stream->size = stream_size;
	stream->datos = malloc(stream_size);
	memset(stream->datos,0,stream_size);

	uint8_t tipo = 31; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t numero_pagina = pedido->pagina;
	uint32_t offset_pagina = pedido->offset;
	uint32_t size_pagina = pedido->size;

	memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stream_size,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&numero_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&offset_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&size_pagina,sizeof(uint32_t));

	return stream;
}

t_stream *serializar_escribir_bytes_de_una_pagina_en_UMC(t_escribir_bytes_de_una_pagina_en_UMC *escritura){

	uint32_t 	tmpsize = 0,
				offset = 0;

	size_t size_bytes_a_escribir = escritura->size+1;	//El campo size es el tamano del string, sin el '\0'. Hablado con @Frabos

	uint32_t 	size_escritura = 	sizeof(uint32_t) +	    //Tamano del numero de pagina
									sizeof(uint32_t) +	    //Tamano del offset
									sizeof(uint32_t) +	    //Tamano del campo datos a escribir
									size_bytes_a_escribir ;	//Tamano del char* de bytes

	uint32_t 	stream_size =	sizeof(uint8_t)	+	//Tamano del tipo
								sizeof(uint32_t)+	//Tamano del largo del stream
								size_escritura;		//Tamano del pedido de pagina

	t_stream *stream = malloc(sizeof(t_stream));
	memset(stream, 0,sizeof(t_stream));

	stream->size = stream_size;
	stream->datos = malloc(stream_size);
	memset(stream->datos,0,stream_size);

	uint8_t tipo = 33; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t numero_pagina = escritura->pagina;
	uint32_t offset_pagina = escritura->offset;
	uint32_t size_pagina = escritura->size;

	memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stream_size,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&numero_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&offset_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&size_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,escritura->buffer,tmpsize=size_bytes_a_escribir);
	offset+=tmpsize;

	char endString='\0';
	memcpy(stream->datos+offset-1,&endString,1);

	return stream;
}

t_stream* serializar_cambio_de_proceso (t_cambio_de_proceso* unCambioDeProceso){

	uint32_t 	tmpsize = 0,
				offset = 0;

	uint32_t 	size_entero_cambio_de_proceso = sizeof(uint32_t);

	uint32_t 	streamSize =	sizeof(uint8_t)	+				//Tamano del tipo
								sizeof(uint32_t)+				//Tamano del largo del stream
								size_entero_cambio_de_proceso;	//Tamano del entero de cambio de proceso

	t_stream *stream = malloc(sizeof(t_stream));
	memset(stream, 0,sizeof(t_stream));

	stream->size = streamSize;
	stream->datos = malloc(streamSize);
	memset(stream->datos,0,streamSize);

	uint8_t tipo = 35; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t entero_cambio_de_proceso = unCambioDeProceso->pid;

	memcpy(stream->datos,&tipo,tmpsize = sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&streamSize,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&entero_cambio_de_proceso,sizeof(uint32_t));

	return stream;
}



//Deserealizar Mensaje
void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case (32):
			estructuraDestino = deserializar_respuesta_bytes_de_una_pagina_a_CPU (datos);
			break;
	case (34):
			estructuraDestino = deserializar_respuesta_escribir_bytes_de_una_pagina_en_UMC (datos);
			break;
	case(36):
			estructuraDestino = deserealizar_respuesta_cambio_de_proceso(datos);
			break;
	case(121):
			estructuraDestino = deserealizar_enviar_PCB_a_CPU(datos);
			break;
	}
	return estructuraDestino;

}

t_respuesta_bytes_de_una_pagina_a_CPU *deserializar_respuesta_bytes_de_una_pagina_a_CPU(char *datos){

	int		tmpsize = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_respuesta_bytes_de_una_pagina_a_CPU *respuesta = malloc(sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));
	memset(respuesta,0, sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));

	for(tamanoDato = 0; (datos+desplazamientoHeader)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	respuesta->bytes_de_una_pagina = malloc(tamanoDato+1);
	memset(respuesta->bytes_de_una_pagina,0,tamanoDato+1);

	memcpy(respuesta->bytes_de_una_pagina, datos+desplazamientoHeader, tmpsize=tamanoDato+1);

	char endString = '\0';
	memcpy(respuesta->bytes_de_una_pagina+tamanoDato,&endString,1);

	return respuesta;
}

t_respuesta_escribir_bytes_de_una_pagina_en_UMC * deserializar_respuesta_escribir_bytes_de_una_pagina_en_UMC(char *datos){

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_respuesta_escribir_bytes_de_una_pagina_en_UMC *respuesta = malloc(sizeof(t_respuesta_escribir_bytes_de_una_pagina_en_UMC));
	memset(respuesta,0, sizeof(t_respuesta_escribir_bytes_de_una_pagina_en_UMC));

	memcpy(&respuesta->escritura_correcta, datos+desplazamientoHeader,sizeof(uint32_t));

	return respuesta;
}

t_respuesta_cambio_de_proceso* deserealizar_respuesta_cambio_de_proceso(char *datos){

	const int desplazamiento_header = 5;

	t_respuesta_cambio_de_proceso *un_cambio_de_proceso = malloc(sizeof(t_respuesta_cambio_de_proceso));
	memset(un_cambio_de_proceso,0,sizeof(t_respuesta_cambio_de_proceso));

	memcpy(&un_cambio_de_proceso->un_numero,datos+desplazamiento_header,sizeof(uint32_t));

	return un_cambio_de_proceso;
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

t_recibir_PCB_de_Kernel *deserealizar_enviar_PCB_a_CPU(char *datos){

	uint32_t	tmpsize = 0,
				offset = 0;

	const int desplazamiento_header = 5;	//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_recibir_PCB_de_Kernel *unPCB = malloc(sizeof(t_recibir_PCB_de_Kernel));
	memset(unPCB,0,sizeof(t_recibir_PCB_de_Kernel));

	memcpy(&unPCB->pid,datos+desplazamiento_header,tmpsize=sizeof(uint32_t));
	offset+=desplazamiento_header;
	offset+=tmpsize;

	memcpy(&unPCB->program_counter,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	uint32_t cantidad_elementos_del_stack = 0;

	memcpy(&cantidad_elementos_del_stack,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	int contador_de_elementos_del_stack = 0;

	unPCB->stack_index = list_create();

	while(contador_de_elementos_del_stack<cantidad_elementos_del_stack){

		uint32_t	posicion_retorno = 0,
					size_valor_de_retorno = 0,
					pagina_direccion_del_dato = 0,
					offset_direccion_del_dato = 0;

		int cantidad_de_variables_en_elemento_del_stack = 0;

		memcpy(&posicion_retorno,datos+offset,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(&size_valor_de_retorno,datos+offset,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(&pagina_direccion_del_dato,datos+offset,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(&offset_direccion_del_dato,datos+offset,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(&cantidad_de_variables_en_elemento_del_stack,datos+offset,tmpsize=sizeof(int));
		offset+=tmpsize;

		t_stack_element *stack_element = malloc(sizeof(t_stack_element));

		stack_element->posicion_retorno = posicion_retorno;
		stack_element->valor_retorno->size = size_valor_de_retorno;
		stack_element->valor_retorno->direccion->offset = offset_direccion_del_dato;
		stack_element->valor_retorno->direccion->pagina = pagina_direccion_del_dato;
		stack_element->variables = list_create();

		int contador_de_variables_en_el_elemento_del_stack = 0;

		while(contador_de_variables_en_el_elemento_del_stack < cantidad_de_variables_en_elemento_del_stack){

			char id;

			uint32_t 	size_dato = 0,
						page_virtual_address = 0,
						offset_virtual_address = 0;

			memcpy(&id,datos+offset,tmpsize=sizeof(char));
			offset+=tmpsize;

			memcpy(&size_dato,datos+offset,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			memcpy(&page_virtual_address,datos+offset,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			memcpy(&offset_virtual_address,datos+offset,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			contador_de_variables_en_el_elemento_del_stack++;

			t_variable *una_variable = malloc(sizeof(t_variable));

			una_variable->id = id;
			una_variable->dato->size = size_dato;
			una_variable->dato->direccion->offset = offset_virtual_address;
			una_variable->dato->direccion->pagina = page_virtual_address;

			list_add(stack_element->variables,una_variable);

		}

		list_add(unPCB->stack_index,stack_element);

		contador_de_elementos_del_stack++;
	}

	memcpy(&unPCB->stack_pointer,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->stack_size,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->used_pages,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->instructions_size,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	int contador_de_instrucciones = 0;

	t_intructions * instrucciones = malloc(sizeof(t_intructions)*(unPCB->instructions_size));
	unPCB->instructions_index = instrucciones;

	while(contador_de_instrucciones < unPCB->instructions_size){

	t_puntero_instruccion primera_instruccion = 0;
	t_size offset_instrucciones = 0;

	memcpy(&primera_instruccion,datos+offset,tmpsize=sizeof(t_puntero_instruccion));
	offset+=tmpsize;

	memcpy(&offset_instrucciones,datos+offset,tmpsize=sizeof(t_size));
	offset+=tmpsize;

	(unPCB->instructions_index)[contador_de_instrucciones] = carga_instructions_index(primera_instruccion,offset_instrucciones);

	contador_de_instrucciones++;

	}

	return unPCB;
}

uint32_t deserealizar_int(char* datos) {

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	uint32_t respuesta = 0;

	memcpy(&respuesta,datos+desplazamientoHeader, sizeof(uint32_t));

	return respuesta;
}


//Funciones Auxiliares
//Dale Frabos
t_list *arma_stack_del_PCB(int elemento_del_stack){

	t_list *stack_element = list_create();

	stack_element->elements_count = elemento_del_stack;
	stack_element->head = NULL;

	return stack_element;
}

t_intructions carga_instructions_index(t_puntero_instruccion primera_instruccion,t_size offset_instrucciones){

	t_intructions instrucciones;

	instrucciones.start = primera_instruccion;
	instrucciones.offset = offset_instrucciones;

	return instrucciones;
}



