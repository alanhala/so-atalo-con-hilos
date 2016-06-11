/*
 * tests.c
 *
 *  Created on: 18/5/2016
 *      Author: utnso
 */
#include "CUnit/Basic.h"
#include "tests.h"
#include "cpu.h"
#include <parser/metadata_program.h>

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
	CU_add_test(prueba, "ocho", test_ejecutar_programa_en_memoria);
	CU_add_test(prueba, "nueve", test_ir_a_label);

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

void test_ir_a_label() {
    mockear_pcb();
    t_PCB *pcb = get_PCB();
    CU_ASSERT_EQUAL(pcb->program_counter, 1);

    irALabel("segundo_label");

    CU_ASSERT_EQUAL(pcb->program_counter, 3);
}

void test_ejecutar_programa_en_memoria() {
    //inicializo PCB
    mockear_pcb();
    t_PCB * pcb=get_PCB();
    pcb->pid = 0;
    pcb->program_counter = 0;

    cambiar_contexto(pcb->pid);

    //Cargo metadata de programa ANSISOP en PCB
    t_metadata_program *metadata = metadata_desde_literal("begin\nvariables c, d\nc=2147483647\nd=224947129\nend\0");
    pcb->instructions_index = metadata->instrucciones_serializado;

    //Ejecuto primera instruccion variables c, d
    execute_next_instruction_for_process();

    t_stack_element *stack_element = list_get(pcb->stack, 0);
    CU_ASSERT_EQUAL(list_size(stack_element->variables), 2);
    t_variable *variable = list_get(stack_element->variables, 0);
    CU_ASSERT_EQUAL(variable->id, 'c');
    variable = list_get(stack_element->variables, 1);
    CU_ASSERT_EQUAL(variable->id, 'd');

    //ejecuto segunda instruccion c=1234
    pcb->program_counter++;
    execute_next_instruction_for_process();
    CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('c')), 2147483647);

    //ejecuto segunda instruccion d=4321
    pcb->program_counter++;
    execute_next_instruction_for_process();
    CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('d')), 224947129);
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
    CU_ASSERT_EQUAL(dato->direccion.pagina, 20);
    CU_ASSERT_EQUAL(dato->direccion.offset, 3);
}

void test_asignar_y_leer_valor_de_una_sola_pagina() {
    mockear_pcb();
    t_PCB * pcb=get_PCB();
    pcb->pid = 0;
    set_PCB(pcb);

    cambiar_contexto(pcb->pid);
    definirVariable('a');
    t_dato_en_memoria *dato_en_memoria = obtenerPosicionVariable('a');
    asignar(dato_en_memoria, 123456789);
    t_valor_variable valor = dereferenciar(dato_en_memoria);

    CU_ASSERT_EQUAL(valor, 123456789);
}

void test_asignar_y_leer_valor_de_varias_paginas() {
    mockear_pcb();
    t_PCB * pcb=get_PCB();
    pcb->pid = 0;
    pcb->stack_free_space_pointer.offset = 3;
    set_PCB(pcb);
    cambiar_contexto(pcb->pid);
    definirVariable('a');
    t_dato_en_memoria *dato_en_memoria = obtenerPosicionVariable('a');
    asignar(dato_en_memoria, 123456789);
    t_valor_variable valor = dereferenciar(dato_en_memoria);

    CU_ASSERT_EQUAL(valor, 123456789);
}

void test_actualizar_next_free_space() {
    mockear_pcb();

    incrementar_next_free_space(4);

    t_PCB *pcb = get_PCB();

    CU_ASSERT_EQUAL(pcb->stack_free_space_pointer.offset, 2);
    CU_ASSERT_EQUAL(pcb->stack_free_space_pointer.pagina, 21);

    incrementar_next_free_space(4);

    CU_ASSERT_EQUAL(pcb->stack_free_space_pointer.offset, 1);
    CU_ASSERT_EQUAL(pcb->stack_free_space_pointer.pagina, 22);
}


void mockear_pcb() {
    t_PCB *pcb = malloc(sizeof(t_PCB));
    pcb->instructions_index = malloc(sizeof(t_indice_instrucciones_elemento)*2);

    t_intructions *indice = pcb->instructions_index;

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
    free_space->pagina = 20;

    pcb->stack_free_space_pointer = *free_space;

    t_stack_element* stack_element = create_stack_element();
    list_add(pcb->stack, stack_element);

    pcb->label_index = list_create();
    t_label_index *label_index_element = malloc(sizeof(t_label_index));
    label_index_element->location = 8;
    label_index_element->name = "inicio";

    list_add(pcb->label_index, label_index_element);

    label_index_element = malloc(sizeof(t_label_index));
    label_index_element->location = 3;
    label_index_element->name = "segundo_label";
    list_add(pcb->label_index, label_index_element);
    set_PCB(pcb);
}

void test_leer_data_de_memoria_con_iteraciones() {
    char* leer_memoria(t_dato_en_memoria *dato) {
	switch(dato->direccion.pagina) {
	    case (1): return "012";
	    case (2): return "abcde";
	    case(4): return "ab";
	    case (3): return "98765";
	}
    }

    t_dato_en_memoria *dato = malloc(sizeof(t_dato_en_memoria));
    dato->size = 15;
    dato->direccion.offset=2;
    dato->direccion.pagina=1;

    char* result = ejecutar_lectura_de_dato_con_iteraciones(leer_memoria, dato, 5);

    CU_ASSERT_STRING_EQUAL(result, "012abcde98765ab");
}
