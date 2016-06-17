/*
 * simulacion.c
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */


#include "CUnit/Basic.h"
#include "../tests.h"
#include "../cpu.h"

void simulacion_uno();
void varios_programas_ansisop();
int correr_simulacion(){

	CU_initialize_registry();
	CU_pSuite simulacion = CU_add_suite("Suite de simulacion", NULL, NULL);
	//CU_add_test(simulacion, "uno", simulacion_uno);
	CU_add_test(simulacion, "varios programas ansisop", varios_programas_ansisop);


	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}
t_PCB *mockear_pcb_simulacion();
void varios_programas_ansisop(){

	    t_PCB * pcb_uno = mockear_pcb_simulacion();
	    t_PCB * pcb_dos = mockear_pcb_simulacion();
	    t_PCB * pcb_tres = mockear_pcb_simulacion();
	    t_PCB * pcb_cuatro = mockear_pcb_simulacion();

	    pcb_uno->pid = 0;
	    pcb_uno->program_counter = 0;
	    pcb_dos->pid = 1;
		pcb_dos->program_counter = 0;
		pcb_tres->pid = 2;
		pcb_tres->program_counter = 0;
		pcb_cuatro->pid = 3;
		pcb_cuatro->program_counter = 0;


	    t_metadata_program *metadata = metadata_desde_literal("begin\nvariables c, d, e\nc=2147483647\nd=224947129\nf\ne <- g\nend\nfunction f\nvariables a\na=1234\nend\nfunction g\nvariables a\na=2\nreturn a\nend");
	    pcb_uno->instructions_index = metadata->instrucciones_serializado;
	    pcb_uno->label_index = get_label_index(metadata);
		pcb_dos->instructions_index = metadata->instrucciones_serializado;
		pcb_dos->label_index = get_label_index(metadata);
		pcb_tres->instructions_index = metadata->instrucciones_serializado;
		pcb_tres->label_index = get_label_index(metadata);
		pcb_cuatro->instructions_index = metadata->instrucciones_serializado;
		pcb_cuatro->label_index = get_label_index(metadata);

		cambiar_contexto(pcb_uno->pid);
		set_PCB(pcb_uno);
		execute_next_instruction_for_process(); //del primer pcb
	    t_stack_element *stack_element = list_get(pcb_uno->stack, 0);
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 3);
	    t_variable *variable = list_get(stack_element->variables, 0);
	    CU_ASSERT_EQUAL(variable->id, 'c');
	    variable = list_get(stack_element->variables, 1);
	    CU_ASSERT_EQUAL(variable->id, 'd');


	    cambiar_contexto(pcb_dos->pid);
	    set_PCB(pcb_dos);
	    execute_next_instruction_for_process(); //del segundo pcb
	    stack_element = list_get(pcb_dos->stack, 0);
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 3);
	    variable = list_get(stack_element->variables, 0);
	    CU_ASSERT_EQUAL(variable->id, 'c');
	    variable = list_get(stack_element->variables, 1);
	    CU_ASSERT_EQUAL(variable->id, 'd');

	    cambiar_contexto(pcb_tres->pid);
		set_PCB(pcb_tres);
		execute_next_instruction_for_process(); //del segundo pcb

		stack_element = list_get(pcb_tres->stack, 0);
		CU_ASSERT_EQUAL(list_size(stack_element->variables), 3);
		variable = list_get(stack_element->variables, 0);
		CU_ASSERT_EQUAL(variable->id, 'c');
		variable = list_get(stack_element->variables, 1);
		CU_ASSERT_EQUAL(variable->id, 'd');

	    cambiar_contexto(pcb_cuatro->pid);
		set_PCB(pcb_cuatro);
		execute_next_instruction_for_process(); //del segundo pcb
		stack_element = list_get(pcb_cuatro->stack, 0);
		CU_ASSERT_EQUAL(list_size(stack_element->variables), 3);
		variable = list_get(stack_element->variables, 0);
		CU_ASSERT_EQUAL(variable->id, 'c');
		variable = list_get(stack_element->variables, 1);
		CU_ASSERT_EQUAL(variable->id, 'd');




		cambiar_contexto(pcb_uno->pid);
		set_PCB(pcb_uno);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('c')), 2147483647);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('d')), 224947129);



	    cambiar_contexto(pcb_dos->pid);
		set_PCB(pcb_dos);
		execute_next_instruction_for_process();
		CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('c')), 2147483647);
		execute_next_instruction_for_process();
		CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('d')), 224947129);

		cambiar_contexto(pcb_tres->pid);
		set_PCB(pcb_tres);
		execute_next_instruction_for_process();
		CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('c')), 2147483647);
		execute_next_instruction_for_process();
		CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('d')), 224947129);

		cambiar_contexto(pcb_cuatro->pid);
		set_PCB(pcb_cuatro);
		execute_next_instruction_for_process();
		CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('c')), 2147483647);
		execute_next_instruction_for_process();
		CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('d')), 224947129);


		cambiar_contexto(pcb_uno->pid);
		set_PCB(pcb_uno);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(2, list_size(pcb_uno->stack));
	    CU_ASSERT_EQUAL(6, pcb_uno->program_counter);
	    stack_element = list_get(pcb_uno->stack, 1);
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 0);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 1);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('a')), 1234);


		cambiar_contexto(pcb_dos->pid);
		set_PCB(pcb_dos);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(2, list_size(pcb_dos->stack));
	    CU_ASSERT_EQUAL(6, pcb_dos->program_counter);
	    stack_element = list_get(pcb_dos->stack, 1);
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 0);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 1);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('a')), 1234);


		cambiar_contexto(pcb_tres->pid);
		set_PCB(pcb_tres);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(2, list_size(pcb_tres->stack));
	    CU_ASSERT_EQUAL(6, pcb_tres->program_counter);
	    stack_element = list_get(pcb_tres->stack, 1);
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 0);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 1);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('a')), 1234);


		cambiar_contexto(pcb_cuatro->pid);
		set_PCB(pcb_cuatro);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(2, list_size(pcb_cuatro->stack));
	    CU_ASSERT_EQUAL(6, pcb_cuatro->program_counter);
	    stack_element = list_get(pcb_cuatro->stack, 1);
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 0);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(list_size(stack_element->variables), 1);
	    execute_next_instruction_for_process();
	    CU_ASSERT_EQUAL(dereferenciar(obtenerPosicionVariable('a')), 1234);



	    cambiar_contexto(pcb_uno->pid);
		set_PCB(pcb_uno);
		execute_next_instruction_for_process();
	    execute_next_instruction_for_process();
	    execute_next_instruction_for_process();
	    execute_next_instruction_for_process();
	    execute_next_instruction_for_process();
	    execute_next_instruction_for_process();
	    int valor_final_e = dereferenciar(obtenerPosicionVariable('e'));
	   // CU_ASSERT_EQUAL(valor_final_e, 4321);
	    CU_ASSERT_EQUAL(valor_final_e, 2);


	    cambiar_contexto(pcb_dos->pid);
		set_PCB(pcb_dos);
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		valor_final_e = dereferenciar(obtenerPosicionVariable('e'));
	   // CU_ASSERT_EQUAL(valor_final_e, 4321);
		CU_ASSERT_EQUAL(valor_final_e, 2);

		cambiar_contexto(pcb_tres->pid);
		set_PCB(pcb_tres);
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		valor_final_e = dereferenciar(obtenerPosicionVariable('e'));
	   // CU_ASSERT_EQUAL(valor_final_e, 4321);
		CU_ASSERT_EQUAL(valor_final_e, 2);

		cambiar_contexto(pcb_cuatro->pid);
		set_PCB(pcb_cuatro);
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		execute_next_instruction_for_process();
		valor_final_e = dereferenciar(obtenerPosicionVariable('e'));
	   // CU_ASSERT_EQUAL(valor_final_e, 4321);
		CU_ASSERT_EQUAL(valor_final_e, 2);


}

void simulacion_uno() {
	connect_to_UMC();

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


    mockear_pcb();

    t_PCB *pcb_dos = get_PCB();
    pcb_dos->stack_free_space_pointer->offset = 3;
    pcb_dos->pid = 2;
    set_PCB(pcb_dos);
    cambiar_contexto(pcb_dos->pid);
    definirVariable('a');
    t_dato_en_memoria *dato_en_memoria_dos = obtenerPosicionVariable('a');
    asignar(dato_en_memoria_dos, 1234);
    t_valor_variable valor_dos = dereferenciar(dato_en_memoria_dos);

    CU_ASSERT_EQUAL(valor_dos, 1234);

}


t_PCB * mockear_pcb_simulacion() {
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
    free_space->offset =3;
    free_space->pagina = 40;

    pcb->stack_free_space_pointer = free_space;

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
    return pcb;
}

