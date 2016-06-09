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
int correr_simulacion(){

	CU_initialize_registry();
	CU_pSuite simulacion = CU_add_suite("Suite de simulacion", NULL, NULL);
	CU_add_test(simulacion, "uno", simulacion_uno);


	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

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
    pcb_dos->stack_free_space_pointer.offset = 3;
    pcb_dos->pid = 2;
    set_PCB(pcb_dos);
    cambiar_contexto(pcb_dos->pid);
    definirVariable('a');
    t_dato_en_memoria *dato_en_memoria_dos = obtenerPosicionVariable('a');
    asignar(dato_en_memoria_dos, 1234);
    t_valor_variable valor_dos = dereferenciar(dato_en_memoria_dos);

    CU_ASSERT_EQUAL(valor_dos, 1234);

}
