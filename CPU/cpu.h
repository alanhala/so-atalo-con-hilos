/*
 * cpu.h
 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_
#endif /* CPU_H_ */

#include <parser/metadata_program.h>
#include <parser/parser.h>
#include "primitive.h"
#include <netdb.h>

typedef struct {
	int numero_pagina;
	t_intructions instruccion;
} t_indice_instrucciones_elemento;

typedef struct {
	uint32_t pagina;
	uint32_t offset;
	uint32_t size;
} t_direccion_virtual_memoria;

typedef struct {
	char id;
	t_direccion_virtual_memoria direccion;
} t_variable;

typedef struct {
	uint32_t cantidad_argumentos;
	t_direccion_virtual_memoria *argumentos;
	uint32_t cantidad_variables;
	t_variable *variables;
	uint32_t posicion_retorno;
	t_direccion_virtual_memoria valor_retorno;
} t_stack_element;

typedef struct {
	uint32_t pid;
	uint32_t program_counter;
	uint32_t paginas_codigo;
	uint32_t cantidad_instrucciones;
	t_indice_instrucciones_elemento* indice_instrucciones;
	uint32_t cantidad_etiquetas;
	//t_indice_etiqueta TODO: implementar esto
	t_stack_element *stack_index;
	t_direccion_virtual_memoria heap_start;
	uint32_t heap_size;
	t_direccion_virtual_memoria heap_first_free_space;
} t_PCB;

void execute_next_instruction_for_process(t_PCB *pcb);

t_indice_instrucciones_elemento get_next_instruction(t_PCB *pcb);

char* obtener_instruccion_de_umc(t_indice_instrucciones_elemento instruccion);

t_puntero definirVariable(t_nombre_variable variable);
