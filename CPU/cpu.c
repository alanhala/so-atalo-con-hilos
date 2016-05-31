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

    t_stream *buffer = (t_stream*)serializar_mensaje(2,pedido);
    send(UMC_DESCRIPTOR, buffer->datos, 20, 0);

    char recv_buffer[50];
    recv(UMC_DESCRIPTOR, recv_buffer, 50, 0);

    t_respuesta_bytes_de_una_pagina_a_CPU *respuesta = malloc(sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));
    respuesta = (t_respuesta_bytes_de_una_pagina_a_CPU*)deserealizar_mensaje(3, recv_buffer);

    return respuesta;
}

t_respuesta_escribir_bytes_de_una_pagina_en_UMC* escribir_en_umc(t_dato_en_memoria dato, void* valor) {
    int umc_socket_descriptor = create_client_socket_descriptor("localhost","2007");
    char * bytes_de_la_pagina = (char*)valor;
    t_escribir_bytes_de_una_pagina_en_UMC *escritura_bytes = malloc(sizeof(t_escribir_bytes_de_una_pagina_en_UMC));
    memset(escritura_bytes,0,sizeof(t_escribir_bytes_de_una_pagina_en_UMC));
    escritura_bytes->pagina = dato.direccion.pagina;
    escritura_bytes->offset = dato.direccion.offset;
    escritura_bytes->size= dato.size;
    escritura_bytes->buffer = bytes_de_la_pagina;

    t_stream *buffer = serializar_mensaje(10,escritura_bytes);

    int bytes= send(umc_socket_descriptor, buffer->datos, 70, 0);

    char recv_buffer[20];
    recv(umc_socket_descriptor, recv_buffer, 20, 0);

    t_respuesta_escribir_bytes_de_una_pagina_en_UMC * respuesta = malloc(sizeof(t_respuesta_escribir_bytes_de_una_pagina_en_UMC));
    return (t_respuesta_escribir_bytes_de_una_pagina_en_UMC*)deserealizar_mensaje(11, recv_buffer);
}

void incrementar_next_free_space() {

};

void set_PCB(t_PCB *new_pcb) {
    pcb = new_pcb;
}

t_PCB* get_PCB() {
    return pcb;
}

t_stack_element* create_stack_element() {
    t_stack_element *stack_element = malloc(sizeof(stack_element));
    stack_element->variables = list_create();
    stack_element->argumentos = list_create();

    return stack_element;
}




set_umc_socket_descriptor(int socket_descriptor){

    UMC_DESCRIPTOR = socket_descriptor;
}


set_kernel_socket_descriptor(int socket_descriptor){
    KERNEL_DESCRIPTOR = socket_descriptor;
}






