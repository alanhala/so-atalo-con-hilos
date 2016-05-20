/*
 * cpu.c

 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */
#include "cpu.h"
#include "protocoloCPU.h"

AnSISOP_funciones functions = {
	.AnSISOP_definirVariable	= definirVariable,
	.AnSISOP_obtenerPosicionVariable= primitive_obtenerPosicionVariable,
	.AnSISOP_dereferenciar	= primitive_dereferenciar,
	.AnSISOP_asignar	= primitive_asignar,
	.AnSISOP_imprimir	= primitive_imprimir,
	.AnSISOP_imprimirTexto	= primitive_imprimirTexto,
};

AnSISOP_kernel kernel_functions = { };

t_PCB *pcb;

void execute_next_instruction_for_process(t_PCB *pcb) {
	t_indice_instrucciones_elemento instruccion = get_next_instruction(pcb);

	char *instruccion_string = obtener_instruccion_de_umc(instruccion);

	analizadorLinea(strdup(instruccion_string), &functions, &kernel_functions);
}


char* obtener_instruccion_de_umc(t_indice_instrucciones_elemento instruccion) {
	return "a = a + b";
}

t_indice_instrucciones_elemento get_next_instruction(t_PCB *pcb) {
	t_indice_instrucciones_elemento *indice = pcb->indice_instrucciones;

	indice += pcb->program_counter;

	return *indice;
}

t_puntero definirVariable(t_nombre_variable variable) {
	printf("definir la variable %c\n", variable);

	t_variable *puntero_variables = pcb->stack_index->variables;
	puntero_variables += pcb->stack_index->cantidad_variables;

	puntero_variables->id = variable;
	puntero_variables->direccion.size = sizeof(uint32_t);
	puntero_variables->direccion.pagina = pcb->heap_first_free_space.pagina;//TODO: no siempre va a entrar la variable en la pagina actual
	puntero_variables->direccion.offset = pcb->heap_first_free_space.offset;
	pcb->stack_index->cantidad_variables++;

	return (t_puntero)puntero_variables;
}


