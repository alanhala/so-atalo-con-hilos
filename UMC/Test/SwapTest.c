/*
 * SwapTest.c
 *
 *  Created on: 24/5/2016
 *      Author: utnso
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include "../memoriaPrincipal.h"
#include "../main.h"
#include "CUnit/Basic.h"
#include "test.h"
#include "SwapTest.h"




void crear_swap_mock(){

	char* datos = malloc(10000000);
	memset(datos, '/0', 10000000);
	SWAP_MOCK = datos;

}

int cargar_nuevo_programa_en_swap_mock(int pid, int paginas_requeridas_del_proceso, char *codigo_programa){
	memcpy(SWAP_MOCK + (pid * MAX_FRAMES_POR_PROCESO * 5), codigo_programa, sizeof(codigo_programa));
	return 0;
}

char * leer_pagina_de_swap_mock(int pid, int pagina){
	char* datos = malloc(TAMANIO_FRAME);
	memcpy(datos, SWAP_MOCK + (pid * MAX_FRAMES_POR_PROCESO * 5) + (pagina * TAMANIO_FRAME),  TAMANIO_FRAME);
	return datos;



}

void escribir_pagina_de_swap_mock(int pid, int pagina, char* datos) {

	memcpy(SWAP_MOCK + (pid * MAX_FRAMES_POR_PROCESO * 5) +(pagina * TAMANIO_FRAME) , datos, TAMANIO_FRAME);
}


void crear_swap_mock_test();

void escribir_varias_paginas_swap_test();
int correr_swap_mock_test(){

CU_initialize_registry();

      CU_pSuite swap_mock = CU_add_suite("Suite de swap mock", NULL, NULL);
	  CU_add_test(swap_mock, "crear swap mock", crear_swap_mock_test);
	  CU_add_test(swap_mock, "escribir varias paginas", escribir_varias_paginas_swap_test);




	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}

void crear_swap_mock_test(){
	inicializar_estructuras();
	crear_swap_mock();

	//size_t swap_mock_size = malloc_usable_size(SWAP_MOCK);
	int swap_mock_size = strlen(SWAP_MOCK);
	CU_ASSERT_TRUE( swap_mock_size== 10000 );
}

void escribir_varias_paginas_swap_test(){
	inicializar_estructuras();
	crear_swap_mock();

	set_tamanio_frame(5);

	escribir_pagina_de_swap_mock(0, 0, "pag0");
	escribir_pagina_de_swap_mock(0, 1, "pag1");
	escribir_pagina_de_swap_mock(0, 2, "pag2");
	escribir_pagina_de_swap_mock(0, 3, "pag3");
	escribir_pagina_de_swap_mock(0, 4, "pag4");
	escribir_pagina_de_swap_mock(0, 10, "pag10");
	escribir_pagina_de_swap_mock(0, 11, "pag11");
	escribir_pagina_de_swap_mock(5, 12, "pag12pag2");

	char * pag2 = leer_pagina_de_swap_mock(0, 2);
	char * pag12_pid5 = leer_pagina_de_swap_mock(5, 12);

	CU_ASSERT_EQUAL( strcmp(pag2, "pag2"), 0 );
	CU_ASSERT_EQUAL( strcmp(pag12_pid5, "pag12"), 0 );

}
