/*
 * cpu.c

 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */
#include "cpu.h"
#include <commons/collections/list.h>

AnSISOP_funciones functions = {
	.AnSISOP_definirVariable	= definirVariable,
	.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable,
	.AnSISOP_dereferenciar	= dereferenciar,
	.AnSISOP_asignar	= asignar,
	.AnSISOP_imprimir	= primitive_imprimir,
	.AnSISOP_imprimirTexto	= primitive_imprimirTexto,
};

AnSISOP_kernel kernel_functions = { };

t_PCB *pcb;

uint32_t tamanio_pagina;

void execute_next_instruction_for_process() {
	t_indice_instrucciones_elemento instruccion = get_next_instruction();

	char *instruccion_string = obtener_instruccion_de_umc(instruccion);

	analizadorLinea(strdup(instruccion_string), &functions, &kernel_functions);
};


char* obtener_instruccion_de_umc(t_indice_instrucciones_elemento instruccion) {
	return "a = a + b";
}

t_indice_instrucciones_elemento get_next_instruction() {
	t_indice_instrucciones_elemento *indice = pcb->indice_instrucciones;

	indice += pcb->program_counter;

	return *indice;
}

t_puntero definirVariable(t_nombre_variable variable) {
	t_stack_element *stack_element = list_get(pcb->stack, list_size(pcb->stack) - 1);

	t_variable *new_variable = malloc(sizeof(t_variable));

	new_variable->id = variable;
	new_variable->dato.size = sizeof(uint32_t);
	new_variable->dato.direccion.pagina = pcb->stack_next_free_space.pagina;
	new_variable->dato.direccion.offset = pcb->stack_next_free_space.offset;

	incrementar_next_free_space(pcb);

	list_add(stack_element->variables, new_variable);

	return (t_puntero)&new_variable->dato;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	t_stack_element *stack_element = list_get(pcb->stack, list_size(pcb->stack) - 1);

	int find_variable(t_variable *var) {
		return var->id == identificador_variable;
	}

	t_variable *variable = list_find(stack_element->variables, (void*)find_variable);

	return (t_puntero) &variable->dato;
}

t_valor_variable dereferenciar(t_puntero direccion_variable) {
    t_dato_en_memoria *direccion = (t_dato_en_memoria*) direccion_variable;

    t_respuesta_bytes_de_una_pagina_a_CPU* respuesta = leer_memoria_de_umc(*direccion);

    return (t_valor_variable)respuesta;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor) {
    t_dato_en_memoria *direccion = (t_variable*) direccion_variable;

    escribir_en_umc(*direccion, valor);
}

t_respuesta_bytes_de_una_pagina_a_CPU* leer_memoria_de_umc(t_dato_en_memoria dato) {

    t_solicitar_bytes_de_una_pagina_a_UMC *pedido = malloc(sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));

    memset(pedido,0,sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));

    pedido->pagina = dato.direccion.pagina;
    pedido->offset = dato.direccion.offset;
    pedido->size = dato.size;

    t_stream *buffer = (t_stream*)serializar_mensaje(31,pedido);

    send(UMC_DESCRIPTOR, buffer->datos, buffer->size, 0);

    t_header *aHeader = malloc(sizeof(t_header));

    char 	buffer_header[5];	//Buffer donde se almacena el header recibido

    int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
        	bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

    recv(UMC_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

    char buffer_recv[buffer_header[1]];

    recv(UMC_DESCRIPTOR, buffer_recv, buffer_header[1], 0);

    t_respuesta_bytes_de_una_pagina_a_CPU *respuesta = malloc(sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));

    respuesta = (t_respuesta_bytes_de_una_pagina_a_CPU*)deserealizar_mensaje(buffer_header[0], buffer_recv);

    return respuesta;
}

t_respuesta_escribir_bytes_de_una_pagina_en_UMC* escribir_en_umc(t_dato_en_memoria dato, void* valor) {

    char *bytes_de_la_pagina = (char *)valor;

    t_escribir_bytes_de_una_pagina_en_UMC *escritura_bytes = malloc(sizeof(t_escribir_bytes_de_una_pagina_en_UMC));

    memset(escritura_bytes,0,sizeof(t_escribir_bytes_de_una_pagina_en_UMC));

    escritura_bytes->pagina = dato.direccion.pagina;
    escritura_bytes->offset = dato.direccion.offset;
    escritura_bytes->size= dato.size;
    escritura_bytes->buffer = bytes_de_la_pagina;

    t_stream *buffer = serializar_mensaje(33,escritura_bytes);

    int bytes = send(UMC_DESCRIPTOR, buffer->datos, buffer->size, 0);

    t_header *aHeader = malloc(sizeof(t_header));

    char 	buffer_header[5];	//Buffer donde se almacena el header recibido

    int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
            bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

    recv(UMC_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

    char buffer_recv[buffer_header[1]];

    recv(UMC_DESCRIPTOR, buffer_recv, buffer_header[1], 0);

    t_respuesta_escribir_bytes_de_una_pagina_en_UMC * respuesta = malloc(sizeof(t_respuesta_escribir_bytes_de_una_pagina_en_UMC));

    return (t_respuesta_escribir_bytes_de_una_pagina_en_UMC*)deserealizar_mensaje(34, buffer_recv);
}

void incrementar_next_free_space(uint32_t size) {
    t_direccion_virtual_memoria direccion = pcb->stack_next_free_space;
    if(direccion.offset + size > tamanio_pagina) {
	pcb->stack_next_free_space.pagina++;
	pcb->stack_next_free_space.offset = 0;
    } else {
	pcb->stack_next_free_space.offset += size;
    }
};

void free_stack_element_memory(t_stack_element *element) {

    void free_memory(void *variable) {
	free(variable);
    }

    list_destroy_and_destroy_elements(element->variables, free_memory);
    list_destroy_and_destroy_elements(element->argumentos, free_memory);
};

void set_PCB(t_PCB *new_pcb) {
    pcb = new_pcb;
}

t_PCB* get_PCB() {
    return pcb;
}

void set_tamanio_pagina(uint32_t tamanio) {
    tamanio_pagina = tamanio;
}

t_stack_element* create_stack_element() {
    t_stack_element *stack_element = malloc(sizeof(stack_element));
    stack_element->variables = list_create();
    stack_element->argumentos = list_create();

    return stack_element;
}


char* ejecutar_lectura_de_dato_con_iteraciones(void*(*closure_lectura)(t_dato_en_memoria*), t_dato_en_memoria* dato, uint32_t tamanio_pagina) {
    if(dato->direccion.offset + dato->size < tamanio_pagina) {
	return closure_lectura(dato);
    }

    char *result = malloc(sizeof(char) * dato->size);
    t_dato_en_memoria aux_dato = *dato;
    int remaining_size = dato->size;
    int desplazamiento_acumulado = 0;

    while(remaining_size > 0) {
	if(remaining_size > tamanio_pagina) {
	    aux_dato.size = tamanio_pagina - aux_dato.direccion.offset;
	} else {
	    aux_dato.size = remaining_size;
	}

	memcpy(result+desplazamiento_acumulado, closure_lectura(&aux_dato), aux_dato.size);

	desplazamiento_acumulado += aux_dato.size;
	aux_dato.direccion.offset = 0;
	remaining_size -= aux_dato.size;
    }

    return result;
}

set_umc_socket_descriptor(int socket_descriptor){

    UMC_DESCRIPTOR = socket_descriptor;
}


set_kernel_socket_descriptor(int socket_descriptor){
    KERNEL_DESCRIPTOR = socket_descriptor;
}






