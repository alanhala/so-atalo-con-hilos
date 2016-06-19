/*
 * tests.h
 *
 *  Created on: 18/5/2016
 *      Author: utnso
 */

#ifndef TESTS_H_
#define TESTS_H_

#include "cpu.h"

int correrTest();

void mockear_pcb();
void obtener_siguiente_instruccion();
void test_definir_variable();
void test_obtener_posicion_variable();
void test_actualizar_next_free_space();
void test_asignar_y_leer_valor_de_una_sola_pagina();
void test_leer_data_de_memoria_con_iteraciones();
void test_asignar_y_leer_valor_de_varias_paginas();
void test_ejecutar_programa_en_memoria();
void test_ir_a_label();
t_list* get_label_index(t_metadata_program* metadata);
t_label_index* create_label_index(char* label_name, int label_location);

#endif /* TESTS_H_ */
