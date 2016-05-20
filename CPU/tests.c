/*
 * tests.c
 *
 *  Created on: 18/5/2016
 *      Author: utnso
 */
#include "CUnit/Basic.h"
#include "tests.h"
#include "cpu.h"

int correrTest(){

	CU_initialize_registry();
	CU_pSuite prueba = CU_add_suite("Suite de prueba", NULL, NULL);
	CU_add_test(prueba, "uno", obtener_siguiente_instruccion);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}

void obtener_siguiente_instruccion() {
	t_PCB *pcb = malloc(sizeof(t_PCB));
	pcb->indice_instrucciones = malloc(sizeof(t_indice_instrucciones_elemento)*2);

	t_indice_instrucciones_elemento *indice = pcb->indice_instrucciones;
	t_indice_instrucciones_elemento *indice_copia = pcb->indice_instrucciones;

	pcb->program_counter = 1;

	indice->numero_pagina = 0;
	indice->instruccion.offset = 15;
	indice->instruccion.start = 0;

	indice++;

	indice->numero_pagina = 1;
	indice->instruccion.offset = 32;
	indice->instruccion.start = 16;

	indice_copia += pcb->program_counter;

	t_indice_instrucciones_elemento next_instruction = get_next_instruction(pcb);

	CU_ASSERT_EQUAL(next_instruction.instruccion.offset, 32);
	CU_ASSERT_EQUAL(next_instruction.instruccion.start, 16);
	CU_ASSERT_EQUAL(next_instruction.numero_pagina, 1);

}
