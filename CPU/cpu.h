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

typedef struct {
	int numero_pagina;
	t_intructions instruccion;
} t_indice_instrucciones_elemento;

typedef struct {
	int pid;
	int program_counter;
	int paginas_codigo;
	int cantidad_instrucciones;
	t_indice_instrucciones_elemento* indice_instrucciones;
	int cantidad_etiquetas;
	//t_indice_etiqueta TODO: implementar esto
	void* stack_index;
} t_PCB;

void execute_next_instruction_for_process(t_PCB *pcb);

t_intructions get_next_instruction(t_PCB *pcb);
