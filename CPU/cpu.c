/*
 * cpu.c

 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */
#include "cpu.h"

AnSISOP_funciones functions = {
	.AnSISOP_definirVariable	= primitive_definirVariable,
	.AnSISOP_obtenerPosicionVariable= primitive_obtenerPosicionVariable,
	.AnSISOP_dereferenciar	= primitive_dereferenciar,
	.AnSISOP_asignar	= primitive_asignar,
	.AnSISOP_imprimir	= primitive_imprimir,
	.AnSISOP_imprimirTexto	= primitive_imprimirTexto,
};

AnSISOP_kernel kernel_functions = { };

void execute_next_instruction_for_process(t_PCB *pcb) {
	t_intructions instruccion = get_next_instruction(pcb);

	//analizadorLinea(strdup(DEFINICION_VARIABLES), &functions, &kernel_functions);
}


t_intructions get_next_instruction(t_PCB *pcb) {
	t_indice_instrucciones_elemento *indice = pcb->indice_instrucciones;

	indice += pcb->program_counter;

	return indice->instruccion;
}
