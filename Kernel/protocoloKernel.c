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
			 stream = serializar_PCB((t_PCB_serializacion *)unaEstructura);
			 break;
	case(133):
			 stream = serializar_finalizar_consola((t_finalizar_programa_en_consola *)unaEstructura);
			 break;
	       }

	return stream;
}

t_stream *serializar_finalizar_consola(t_finalizar_programa_en_consola *unaEstructura){

	uint32_t 	tmpsize = 0,
				offset = 0;

	uint32_t 	size_entero_finalizar = sizeof(uint32_t);

	uint32_t 	streamSize =	sizeof(uint8_t)	+				//Tamano del tipo
								sizeof(uint32_t)+				//Tamano del largo del stream
								size_entero_finalizar;	//Tamano del entero de cambio de proceso

	t_stream *stream = malloc(sizeof(t_stream));
	memset(stream, 0,sizeof(t_stream));

	stream->size = streamSize;
	stream->datos = malloc(streamSize);
	memset(stream->datos,0,streamSize);

	uint8_t tipo = 133; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t entero_finalizar = unaEstructura->motivo;

	memcpy(stream->datos,&tipo,tmpsize = sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&streamSize,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&entero_finalizar,sizeof(uint32_t));

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

t_stream *serializar_PCB(t_PCB_serializacion *unPCB){

	uint32_t	tmpsize = 0,
				offset = 0;

	//Tamano del Stack -- Inicio
	uint32_t cantidad_elementos_stack = unPCB->stack_index->elements_count;
	uint32_t tamano_total_stack = 0;
	void calcular_tamano_de_un_elemento_del_stack(t_stack_element *stack_element){

		uint32_t tamano_fijo_del_elemento =	sizeof(uint32_t) +	//Posicion de retorno
											sizeof(uint32_t) +	//Size valor de retorno
											sizeof(uint32_t) + 	//Pagina
											sizeof(uint32_t) + 	//Offset
											sizeof(uint32_t); 	//Cantidad de Variables

		uint32_t cantidad_de_variables_del_elemento = stack_element->variables->elements_count;
		//uint32_t tamano_de_variables_del_elemento =	sizeof(t_variable) * cantidad_de_variables_del_elemento;
		uint32_t tamano_de_variables_del_elemento =	(sizeof(t_nombre_variable)+3*sizeof(uint32_t)) * cantidad_de_variables_del_elemento;

		uint32_t tamano_del_elemento = tamano_fijo_del_elemento + tamano_de_variables_del_elemento;

		tamano_total_stack += tamano_del_elemento;
	}
	list_iterate(unPCB->stack_index,(void *)calcular_tamano_de_un_elemento_del_stack);
	//Tamano del Stack -- Fin

	//Tamano de la lista de labels -- Inicio
	uint32_t tamano_total_de_la_lista_de_labels = 0;
	void calcular_tamano_de_un_label (t_label_index *un_label){

		uint32_t tamano_del_name = strlen(un_label->name)+1;
		uint32_t tamano_de_location = sizeof(uint32_t);

		uint32_t tamano_del_label = tamano_del_name + tamano_de_location;

		tamano_total_de_la_lista_de_labels += tamano_del_label;
	}
	list_iterate(unPCB->label_index,(void *)calcular_tamano_de_un_label);
	uint32_t acumula_labels = list_size(unPCB->label_index);
	//Tamano de la lista de labels -- Fin

	uint32_t sizeof_instruccion = unPCB->instructions_size * obtiene_sizeof_instrucciones(unPCB->instructions_index);

	uint32_t sizeof_mensaje_del_pcb = strlen(unPCB->valor_mensaje)+1;

	uint32_t sizePCB =	sizeof(uint32_t)  +		//Process ID
						sizeof(uint32_t)  +		//Program Counter
						tamano_total_stack+		//Tamano total de la lista de stack elements
						sizeof(uint32_t)  +		//Stack Pointer
						sizeof(uint32_t)  +		//Stack Size
						sizeof(uint32_t)  +		//Used Pages
						sizeof(uint32_t)  +		//Instructions Size
						sizeof_instruccion+		//Tamano de las instrucciones
						sizeof(uint32_t)  +		//Tamano del flag program_finished
						sizeof(uint32_t)  +		//Quantum
						sizeof(uint32_t)  +		//Quantum Sleep
						sizeof(t_virtual_address) +	//Tamano de la direccion virtual de memoria
						sizeof(uint32_t)  +		//Tamano de la cantidad de elementos de la lista de labels
						tamano_total_de_la_lista_de_labels	+	//Tamano de la lista de labels
						sizeof(uint32_t)  + 	//Tamano del codigo del mensaje
						sizeof_mensaje_del_pcb				+	//Tamano del mensaje del pcb
						sizeof(uint32_t)  +		//Tamano de la cantidad de operaciones del pcb
						sizeof(uint32_t)  +		//Tamano del resultado del mensaje
						sizeof(uint32_t);		//Valor de la variable compartida


	uint32_t stream_size = 	sizeof(uint8_t) +	//Tamano del tipo
							sizeof(uint32_t)+	//Tamano del length del mensaje
							sizeof(uint32_t)+	//Tamano para la cantidad de elementos del stack
							sizePCB;

	t_stream *stream = malloc(sizeof(t_stream));
	memset(stream,0,sizeof(t_stream));

	stream->size = stream_size;

	stream->datos = malloc(stream_size);
	memset(stream->datos,0,stream_size);

	uint8_t		tipo = 121;

	uint32_t 	pid = unPCB->pid,
				program_counter = unPCB->program_counter,
				stack_size = unPCB->stack_size,
				used_pages = unPCB->used_pages,
				instructions_size = unPCB->instructions_size,
				program_finished = unPCB->program_finished,
				quantum = unPCB->quantum,
				quantum_sleep = unPCB->quantum_sleep,
				pagina = unPCB->stack_last_address->page,
				offset_direccion_virtual = unPCB->stack_last_address->offset,
				codigo_del_mensaje = unPCB->mensaje,
				cantidad_de_operaciones = unPCB->cantidad_operaciones,
				resultado_del_mensaje = unPCB->resultado_mensaje,
				valor_de_la_variable_compartida = unPCB->valor_de_la_variable_compartida;


	memcpy(stream->datos,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&stream_size,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&pid,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&program_counter,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&cantidad_elementos_stack,tmpsize=sizeof(int));
	offset+=tmpsize;

	void serializa_lista_de_elementos_de_la_pila(t_stack_element *stack_element){

		uint32_t posicion_retorno = stack_element->posicion_retorno;

		t_dato_en_memoria * valor_de_retorno = stack_element->valor_retorno;
		uint32_t size_valor_de_retorno = valor_de_retorno->size;

		t_virtual_address *direccion_del_dato = valor_de_retorno->direccion;
		uint32_t pagina_direccion_del_dato = direccion_del_dato->page;
		uint32_t offset_direccion_del_dato = direccion_del_dato->offset;

		int cantidad_de_variables_en_elemento_del_stack = stack_element->variables->elements_count;

		memcpy(stream->datos+offset,&posicion_retorno,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&size_valor_de_retorno,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&pagina_direccion_del_dato,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&offset_direccion_del_dato,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		memcpy(stream->datos+offset,&cantidad_de_variables_en_elemento_del_stack,tmpsize=sizeof(int));
		offset+=tmpsize;

		void serializa_lista_de_variables_del_elemento_de_la_pila(t_variable *una_variable){

			char id = una_variable->id;

			t_dato_en_memoria * dato = una_variable->dato;
			uint32_t size_dato = dato->size;

			t_virtual_address *virtual_address = dato->direccion;
			uint32_t page_virtual_address = virtual_address->page;
			uint32_t offset_virtual_address = virtual_address->offset;

			memcpy(stream->datos+offset,&id,tmpsize=sizeof(char));
			offset+=tmpsize;

			memcpy(stream->datos+offset,&size_dato,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			memcpy(stream->datos+offset,&page_virtual_address,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			memcpy(stream->datos+offset,&offset_virtual_address,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;
		}

		list_iterate(stack_element->variables,(void *)serializa_lista_de_variables_del_elemento_de_la_pila);
	}

	list_iterate(unPCB->stack_index,(void*)serializa_lista_de_elementos_de_la_pila);

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

	memcpy(stream->datos+offset,&program_finished,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&quantum,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&quantum_sleep,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&offset_direccion_virtual,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&acumula_labels,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	void serializa_un_label(t_label_index *un_label){

		char* nombre_del_label = un_label->name;
		uint32_t tamano_del_nombre_del_label = strlen(nombre_del_label)+1;
		uint32_t location = un_label->location;

		memcpy(stream->datos+offset,nombre_del_label,tmpsize=tamano_del_nombre_del_label);
		offset+=tmpsize;

		char end_string = '\0';
		memcpy(stream->datos+offset-1,&end_string,1);

		memcpy(stream->datos+offset,&location,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

	}
	list_iterate(unPCB->label_index,(void *)serializa_un_label);

	memcpy(stream->datos+offset,&codigo_del_mensaje,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,unPCB->valor_mensaje,tmpsize=sizeof_mensaje_del_pcb);
	offset+=tmpsize;

	char end_string = '\0';
	memcpy(stream->datos+offset-1,&end_string,1);

	memcpy(stream->datos+offset,&cantidad_de_operaciones,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&resultado_del_mensaje,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&valor_de_la_variable_compartida,sizeof(uint32_t));

	return stream;
}


//Deserealizacion
void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	 case (62):
		   estructuraDestino = deserealizar_respuesta_iniciar_programa_en_UMC(datos);
		   break;
	 case (64):
		   estructuraDestino = deserealizar_respuesta_finalizar_programa_en_UMC(datos);
		   break;
	 case (91):
		   estructuraDestino = deserealizar_iniciar_programa_en_kernel(datos);
		   break;
	 case (121):
		   estructuraDestino = deserializar_PCB(datos);
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

t_PCB_serializacion *deserializar_PCB(char *datos){

	uint32_t	tmpsize = 0,
				offset = 0;

	const int desplazamiento_header = 5;	//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_PCB_serializacion *unPCB = malloc(sizeof(t_PCB_serializacion));
	memset(unPCB,0,sizeof(t_PCB_serializacion));

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

		stack_element->valor_retorno = malloc(sizeof(t_stack_element));
		stack_element->valor_retorno->size = size_valor_de_retorno;

		stack_element->valor_retorno->direccion = malloc(sizeof(t_virtual_address));
		stack_element->valor_retorno->direccion->offset = offset_direccion_del_dato;
		stack_element->valor_retorno->direccion->page = pagina_direccion_del_dato;

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

			memcpy(&offset_virtual_address,datos+offset,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			memcpy(&page_virtual_address,datos+offset,tmpsize=sizeof(uint32_t));
			offset+=tmpsize;

			contador_de_variables_en_el_elemento_del_stack++;

			t_variable *una_variable = malloc(sizeof(t_variable));
			una_variable->dato = malloc(sizeof(t_dato_en_memoria));
			una_variable->dato->direccion = malloc(sizeof(t_virtual_address));

			una_variable->id = id;
			una_variable->dato->size = size_dato;
			una_variable->dato->direccion->offset = offset_virtual_address;
			una_variable->dato->direccion->page = page_virtual_address;

			list_add(stack_element->variables,una_variable);

		}

		list_add(unPCB->stack_index,stack_element);

		contador_de_elementos_del_stack++;
	}

	memcpy(&unPCB->stack_size,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->used_pages,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->instructions_size,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	int contador_de_instrucciones = 0;

	unPCB->instructions_index = malloc(sizeof(t_intructions)*(unPCB->instructions_size));

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

	memcpy(&unPCB->program_finished,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->quantum,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->quantum_sleep,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	unPCB->stack_last_address = malloc(sizeof(t_virtual_address));

	memcpy(&unPCB->stack_last_address->page,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->stack_last_address->offset,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	uint32_t cantidad_de_labels = 0;

	memcpy(&cantidad_de_labels,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	uint32_t contador_de_labels = 0;

	unPCB->label_index = list_create();

	while (contador_de_labels < cantidad_de_labels){

		t_label_index *un_label = malloc(sizeof(t_label_index));

		uint32_t tamanoDato;
		for(tamanoDato = 0; (datos+offset)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

		un_label->name = malloc(tamanoDato+1);

		memcpy(un_label->name,datos+offset,tmpsize=tamanoDato+1);
		offset+=tmpsize;

		char end_string = '\0';
		memcpy(un_label->name+tamanoDato,&end_string,1);

		memcpy(&un_label->location,datos+offset,tmpsize=sizeof(uint32_t));
		offset+=tmpsize;

		contador_de_labels++;

		list_add(unPCB->label_index,un_label);
	}

	memcpy(&unPCB->mensaje,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	int tamanoDato;
	for(tamanoDato = 0; (datos+offset)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	unPCB->valor_mensaje = malloc(tamanoDato+1);
	memcpy(unPCB->valor_mensaje,datos+offset,tmpsize=tamanoDato+1);
	offset+=tmpsize;

	char end_string = '\0';
	memcpy(unPCB->valor_mensaje+tamanoDato,&end_string,1);

	memcpy(&unPCB->cantidad_operaciones,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->resultado_mensaje,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->valor_de_la_variable_compartida,datos+offset,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&unPCB->cpu_unplugged,datos+offset,sizeof(uint32_t));

	return unPCB;
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

t_puntero_instruccion obtiene_primera_instruccion(t_intructions instruccion){

	t_puntero_instruccion una_instruccion = instruccion.start;

	return una_instruccion;
}

t_size obtiene_offset (t_intructions instruccion){

	t_size offset = instruccion.offset;

	return offset;
}

t_intructions carga_instructions_index(t_puntero_instruccion primera_instruccion,t_size offset_instrucciones){

	t_intructions instrucciones;

	instrucciones.start = primera_instruccion;
	instrucciones.offset = offset_instrucciones;

	return instrucciones;
}
