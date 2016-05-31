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
	CU_add_test(prueba, "dos", test_definir_variable);
	CU_add_test(prueba, "tres", test_obtener_posicion_variable);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}

void obtener_siguiente_instruccion() {
	mockear_pcb();

	t_indice_instrucciones_elemento next_instruction = get_next_instruction();

	CU_ASSERT_EQUAL(next_instruction.instruccion.offset, 32);
	CU_ASSERT_EQUAL(next_instruction.instruccion.start, 16);
	CU_ASSERT_EQUAL(next_instruction.numero_pagina, 1);
}

void test_definir_variable() {
    mockear_pcb();

    definirVariable('a');

    t_PCB *pcb = get_PCB();

    t_stack_element *stack_element = list_get(pcb->stack, 0);
    t_variable *variable = list_get(stack_element->variables, 0);

    CU_ASSERT_EQUAL(variable->id, 'a');
    CU_ASSERT_EQUAL(list_size(stack_element->variables), 1);

    definirVariable('b');

    variable = list_get(stack_element->variables, 1);

    CU_ASSERT_EQUAL(variable->id, 'b');
    CU_ASSERT_EQUAL(list_size(stack_element->variables), 2);
}


void test_obtener_posicion_variable() {
    mockear_pcb();

    definirVariable('a');

    t_PCB *pcb = get_PCB();

    t_dato_en_memoria *dato = obtenerPosicionVariable('a');
    CU_ASSERT_EQUAL(dato->size, sizeof(uint32_t));
    CU_ASSERT_EQUAL(dato->direccion.pagina, 8);
    CU_ASSERT_EQUAL(dato->direccion.offset, 30);
}

void test_asignar_y_leer_valor() {
    mockear_pcb();
    definirVariable('a');
    t_dato_en_memoria *dato_en_memoria = obtenerPosicionVariable('a');
    asignar(dato_en_memoria, 1234);
    t_valor_variable valor = dereferenciar(dato_en_memoria);

    CU_ASSERT_EQUAL(valor, 1234);

}


void mockear_pcb() {
    t_PCB *pcb = malloc(sizeof(t_PCB));
    pcb->indice_instrucciones = malloc(sizeof(t_indice_instrucciones_elemento)*2);

    t_indice_instrucciones_elemento *indice = pcb->indice_instrucciones;

    pcb->program_counter = 1;

    indice->numero_pagina = 0;
    indice->instruccion.offset = 15;
    indice->instruccion.start = 0;

    indice++;

    indice->numero_pagina = 1;
    indice->instruccion.offset = 32;
    indice->instruccion.start = 16;

    pcb->stack = list_create();


    t_direccion_virtual_memoria *free_space = malloc(sizeof(t_direccion_virtual_memoria));
    free_space->offset = 30;
    free_space->pagina = 8;

    pcb->stack_next_free_space = *free_space;

    t_stack_element* stack_element = create_stack_element();
    list_add(pcb->stack, stack_element);

    set_PCB(pcb);
}

