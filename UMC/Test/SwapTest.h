/*
 * SwapTest.h
 *
 *  Created on: 24/5/2016
 *      Author: utnso
 */

#ifndef TEST_SWAPTEST_H_
#define TEST_SWAPTEST_H_

char * SWAP_MOCK;

void crear_swap_mock();
int cargar_nuevo_programa_en_swap_mock(int pid, int paginas_requeridas_del_proceso, char *codigo_programa);
char * leer_pagina_de_swap_mock(int pid, int pagina);
void escribir_pagina_de_swap_mock(int pid, int pagina, char* datos);



#endif /* TEST_SWAPTEST_H_ */
