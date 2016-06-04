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
	CU_add_test(prueba, "cuatro", test_actualizar_next_free_space);
	CU_add_test(prueba, "cinco", test_leer_data_de_memoria_con_iteraciones);
	CU_add_test(prueba, "seis", test_asignar_y_leer_valor_de_una_sola_pagina);
	CU_add_test(prueba, "siete", test_asignar_y_leer_valor_de_varias_paginas);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

void obtener_siguiente_instruccion() {
	mockear_pcb();

	t_dato_en_memoria next_instruction = get_next_instruction();

	CU_ASSERT_EQUAL(next_instruction.size, 32);
	CU_ASSERT_EQUAL(next_instruction.direccion.offset, 1);
	CU_ASSERT_EQUAL(next_instruction.direccion.pagina, 3);
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
    CU_ASSERT_EQUAL(dato->direccion.pagina, 3);
    CU_ASSERT_EQUAL(dato->direccion.offset, 3);
}

void test_asignar_y_leer_valor_de_una_sola_pagina() {
    mockear_pcb();
    t_PCB * pcb=get_PCB();
    pcb->pid = 1;
    set_PCB(pcb);

    cambiar_contexto(pcb->pid);
    definirVariable('a');
    t_dato_en_memoria *dato_en_memoria = obtenerPosicionVariable('a');
    asignar(dato_en_memoria, 1234);
    t_valor_variable valor = dereferenciar(dato_en_memoria);

    CU_ASSERT_EQUAL(valor, 1234);
}

void test_asignar_y_leer_valor_de_varias_paginas() {
    mockear_pcb();
    t_PCB * pcb=get_PCB();
    pcb->pid = 1;
    pcb->stack_next_free_space.offset = 3;
    set_PCB(pcb);
    cambiar_contexto(pcb->pid);
    definirVariable('a');
    t_dato_en_memoria *dato_en_memoria = obtenerPosicionVariable('a');
    asignar(dato_en_memoria, 1234);
    t_valor_variable valor = dereferenciar(dato_en_memoria);

    CU_ASSERT_EQUAL(valor, 1234);
}

void test_actualizar_next_free_space() {
    mockear_pcb();

    incrementar_next_free_space(4);

    t_PCB *pcb = get_PCB();

    CU_ASSERT_EQUAL(pcb->stack_next_free_space.offset, 2);
    CU_ASSERT_EQUAL(pcb->stack_next_free_space.pagina, 4);

    incrementar_next_free_space(4);

    CU_ASSERT_EQUAL(pcb->stack_next_free_space.offset, 1);
    CU_ASSERT_EQUAL(pcb->stack_next_free_space.pagina, 5);
}


void mockear_pcb() {
    t_PCB *pcb = malloc(sizeof(t_PCB));
    pcb->indice_instrucciones = malloc(sizeof(t_indice_instrucciones_elemento)*2);

    t_intructions *indice = pcb->indice_instrucciones;

    pcb->program_counter = 1;

    indice->start = 0;
    indice->offset = 16;

    indice++;

    indice->start = 16;
    indice->offset = 32;

    pcb->stack = list_create();

    set_tamanio_pagina(5);

    t_direccion_virtual_memoria *free_space = malloc(sizeof(t_direccion_virtual_memoria));
    free_space->offset = 3;
    free_space->pagina = 3;

    pcb->stack_next_free_space = *free_space;

    t_stack_element* stack_element = create_stack_element();
    list_add(pcb->stack, stack_element);

    set_PCB(pcb);
}

void test_leer_data_de_memoria_con_iteraciones() {
    char* leer_memoria(t_dato_en_memoria *dato) {
	switch(dato->direccion.pagina) {
	    case (1): return "0123456789";
	    case (2):
	    case(4): return "abcdefgh";
	    case (3): return "987654";
	}
    }

    t_dato_en_memoria *dato = malloc(sizeof(t_dato_en_memoria));
    dato->size = 15;
    dato->direccion.offset=2;
    dato->direccion.pagina=1;

    char* result = ejecutar_lectura_de_dato_con_iteraciones(leer_memoria, dato, 5);

    CU_ASSERT_STRING_EQUAL(result, "012abcde98765ab");
}
