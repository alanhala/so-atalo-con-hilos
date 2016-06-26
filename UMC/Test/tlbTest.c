/*
 * tlbTest.c
 *
 *  Created on: 1/6/2016
 *      Author: utnso
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <semaphore.h>
#include "../memoriaPrincipal.h"
#include "../main.h"
#include "CUnit/Basic.h"
#include "test.h"
#include "tlbTest.h"

void tlb_test_1();
void isPrint();
void algoritmo_clock_modificado();
void algoritmo_clock();

int correr_test_tlb(){
	CU_initialize_registry();

      //CU_pSuite tlb = CU_add_suite("Suite de TLB", NULL, NULL);
	  //CU_add_test(tlb , "tlb test 1", tlb_test_1); //IMPORATENTE. CANTIDAD ENTRADAS TLB TIENE QUE SER 5
      //CU_add_test(tlb , "is print", isPrint);
	  CU_pSuite algoritmos_reemplazo = CU_add_suite("Suite de algoritmos reemplazo", NULL, NULL);
//      CU_add_test(algoritmos_reemplazo , "clock modificado", algoritmo_clock_modificado);
      CU_add_test(algoritmos_reemplazo , "clock", algoritmo_clock);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();



	  return CU_get_error();

}

void algoritmo_clock(){

	//IMPORTANTE VER QUE EL ARCHIVO DE CONFIGURACION TENGA EL MAX FRAME POR PROCESO =3

	inicializar_semaforos();
	lista_cpu_context = list_create();
	set_tamanio_frame(1);
	set_cantidad_frames(50);
	TAMANIO_MEMORIA_PRINCIPAL = TAMANIO_FRAME * CANTIDAD_FRAMES;
	crear_memoria_principal();
	set_cantidad_entradas_tlb(0);
	TLB = crear_tlb();
	lista_tabla_de_paginas = list_create();
	crear_lista_frames();
	set_max_frames_por_proceso(3);
	CANTIDAD_MAXIMA_PROGRAMAS = 5;
	crear_swap_mock();
	set_algoritmo_reemplazo("clock");
	set_retardo(1);
	cargar_nuevo_programa(0, 8, "12345678");

	//2  3  2  1  5  2 4 5 3 2 5 2
	//2  3  2  1  5  2 4 5 3 2 52
	leer_pagina_de_programa(0, 2, 0, 1);
	leer_pagina_de_programa(0, 3, 0, 1);
	leer_pagina_de_programa(0, 2, 0, 1);
	leer_pagina_de_programa(0, 1, 0, 1);
	leer_pagina_de_programa(0, 5, 0, 1);
	leer_pagina_de_programa(0, 2, 0, 1);
	leer_pagina_de_programa(0, 4, 0, 1);
	leer_pagina_de_programa(0, 5, 0, 1);
	leer_pagina_de_programa(0, 3, 0, 1);
//	leer_pagina_de_programa(0, 2, 0, 1);
//	leer_pagina_de_programa(0, 5, 0, 1);
//	leer_pagina_de_programa(0, 2, 0, 1);


	dump_structs(0);
}


void algoritmo_clock_modificado(){
	//IMPORTANTE VER QUE EL ARCHIVO DE CONFIGURACION TENGA EL MAX FRAME POR PROCESO = 4


	inicializar_semaforos();
	lista_cpu_context = list_create();
	set_tamanio_frame(1);
	set_cantidad_frames(50);
	TAMANIO_MEMORIA_PRINCIPAL = TAMANIO_FRAME * CANTIDAD_FRAMES;
	crear_memoria_principal();
	set_cantidad_entradas_tlb(0);
	TLB = crear_tlb();
	lista_tabla_de_paginas = list_create();
	crear_lista_frames();
	set_max_frames_por_proceso(4);
	CANTIDAD_MAXIMA_PROGRAMAS = 5;
	crear_swap_mock();
	set_algoritmo_reemplazo("ClockM");
	set_retardo(1);
	cargar_nuevo_programa(0, 20, "12345123451234512345");




	escribir_pagina_de_programa(0, 14, 0, 1, "x");
	leer_pagina_de_programa(0, 17, 0, 1);
	escribir_pagina_de_programa(0, 19, 0, 1, "p");


	leer_pagina_de_programa(0, 0, 0, 1);
	escribir_pagina_de_programa(0, 15, 0, 1, "p");
	leer_pagina_de_programa(0, 12, 0, 1);
	escribir_pagina_de_programa(0, 17, 0, 1, "p");
	leer_pagina_de_programa(0, 15, 0, 1);
	escribir_pagina_de_programa(0, 19, 0, 1, "p");

	dump_structs(0);
	while(1);






}


void isPrint(){
		set_test();//para usar mock
		set_cantidad_entradas_tlb(5);
		inicializar_estructuras();
		set_algoritmo_reemplazo("ClockM");
		crear_swap_mock();

		char * codigo_pid0= "0pg000pg010pg020pg030pg040pg050pg060pg070pg080pg090pg100pg110pg120pg130pg14";
		char * codigo_pid5 = "5pg005pg015pg025pg035pg045pg055pg065pg075pg085pg095pg105pg115pg125pg135pg14";


		int paginas_necesarias = (30);
		set_max_frames_por_proceso(5);
		cargar_nuevo_programa(0, paginas_necesarias, codigo_pid0);
		cargar_nuevo_programa(5, paginas_necesarias, codigo_pid5);


		char * lectura =  leer_pagina_de_programa(5, 3, 0, TAMANIO_FRAME);
		printf("%s\n", lectura);
		int i = 0;
		while (lectura[i] != '\0') {
		  if (isprint(lectura[i]))
			  printf("%c    ", lectura[i]);
		  else
			  printf("~");
		  i++;
		}
		printf("\n");
		i = 0;
		while (lectura[i] != '\0') {
		  printf("%02x   ", (unsigned int) lectura[i]);
		  i++;
		}
		printf("\n");
//
//		while(*lectura)
//		    printf("%02x   ", (unsigned int) *lectura++);
//		  printf("\n");
//		if (isprint(lectura))
//			printeable=1;

//		lectura =  leer_pagina_de_programa(5, 6, 0, TAMANIO_FRAME);
//		lectura =  leer_pagina_de_programa(0, 9, 0, TAMANIO_FRAME);
//		lectura =  leer_pagina_de_programa(0, 8, 0, TAMANIO_FRAME);
//		lectura =  leer_pagina_de_programa(0, 2, 0, TAMANIO_FRAME);
//



}

void tlb_test_1(){


		set_test();//para usar mock
		set_cantidad_entradas_tlb(5);
		inicializar_estructuras();
		set_algoritmo_reemplazo("ClockM");
		crear_swap_mock();

		char * codigo_pid0= "0pg000pg010pg020pg030pg040pg050pg060pg070pg080pg090pg100pg110pg120pg130pg14";
		char * codigo_pid5 = "5pg005pg015pg025pg035pg045pg055pg065pg075pg085pg095pg105pg115pg125pg135pg14";


		int paginas_necesarias = (30);
		set_max_frames_por_proceso(5);
		cargar_nuevo_programa(0, paginas_necesarias, codigo_pid0);
		cargar_nuevo_programa(5, paginas_necesarias, codigo_pid5);


		char * lectura =  leer_pagina_de_programa(5, 3, 0, TAMANIO_FRAME);
		lectura =  leer_pagina_de_programa(5, 6, 0, TAMANIO_FRAME);
		lectura =  leer_pagina_de_programa(0, 9, 0, TAMANIO_FRAME);
		lectura =  leer_pagina_de_programa(0, 8, 0, TAMANIO_FRAME);
		lectura =  leer_pagina_de_programa(0, 2, 0, TAMANIO_FRAME);


		CU_ASSERT_EQUAL((TLB->entradas[0]).pid , 5);
		CU_ASSERT_EQUAL((TLB->entradas[1]).pid , 5);
		CU_ASSERT_EQUAL((TLB->entradas[2]).pid , 0);
		CU_ASSERT_EQUAL((TLB->entradas[3]).pid , 0);
		CU_ASSERT_EQUAL((TLB->entradas[4]).pid , 0);


		CU_ASSERT_EQUAL((TLB->entradas[0]).lru , 4);
		CU_ASSERT_EQUAL((TLB->entradas[4]).lru , 0);

		lectura =  leer_pagina_de_programa(0, 2, 0, TAMANIO_FRAME);
		CU_ASSERT_EQUAL((TLB->entradas[0]).lru , 5);
		lectura =  leer_pagina_de_programa(5, 8, 0, TAMANIO_FRAME);
		CU_ASSERT_EQUAL((TLB->entradas[0]).lru , 0);
		CU_ASSERT_EQUAL((TLB->entradas[0]).pid , 5);
		CU_ASSERT_EQUAL((TLB->entradas[0]).pagina , 8);
		CU_ASSERT_EQUAL((TLB->entradas[1]).lru , 5);
		CU_ASSERT_EQUAL((TLB->entradas[2]).lru , 4);
		lectura =  leer_pagina_de_programa(0, 9, 0, TAMANIO_FRAME);
		CU_ASSERT_EQUAL((TLB->entradas[2]).lru , 0);

		/*
		int lecturas_pid5=0;
		int resultado_lectura_pid5 = 0;
		for(lecturas_pid5; lecturas_pid5 < 16; lecturas_pid5 ++){
			char * lo_que_leo_pid5 =  leer_pagina_de_programa(5, lecturas_pid5, 0, TAMANIO_FRAME);
			printf("%s\n", lo_que_leo_pid5);
			if (!strcmp(lo_que_leo_pid5, "~/-1"))
				resultado_lectura_pid5 = -1;
		}


		int lecturas=0;
		int resultado_lectura = 0;
		for(lecturas; lecturas < 16; lecturas ++){
			char * lo_que_leo =  leer_pagina_de_programa(0, lecturas, 0, TAMANIO_FRAME);
			printf("%s\n", lo_que_leo);
			if (!strcmp(lo_que_leo, "~/-1"))
				resultado_lectura = -1;
		}


		char * lectura_una_pagina =  leer_pagina_de_programa(0, 0, 0, TAMANIO_FRAME);
		char * lectura_una_pagina_2 =  leer_pagina_de_programa(0, 2, 0, TAMANIO_FRAME);
		char * lectura_una_pagina_10 =  leer_pagina_de_programa(0, 10, 0, TAMANIO_FRAME);

		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina, "0pg00") , 0);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_2, "0pg02") , 0);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_10, "0pg10") , 0);
		escribir_pagina_de_programa(0, 12, 0, TAMANIO_FRAME, "piatti");
		char * lectura_una_pagina_12 =  leer_pagina_de_programa(0, 12, 0, TAMANIO_FRAME);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_12, "piatt") , 0);


		escribir_pagina_de_programa(5, 8, 0, TAMANIO_FRAME, "tony");
		escribir_pagina_de_programa(5, 10, 0, TAMANIO_FRAME, "to10");
		escribir_pagina_de_programa(5, 3, 0, TAMANIO_FRAME, "ton3");
		char * lectura_una_pagina_8_pid5 =  leer_pagina_de_programa(5, 8, 0, TAMANIO_FRAME);
		char * lectura_una_pagina_10_pid5 =  leer_pagina_de_programa(5, 10, 0, TAMANIO_FRAME);
		char * lectura_una_pagina_3_pid5 =  leer_pagina_de_programa(5, 3, 0, TAMANIO_FRAME);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_8_pid5, "tony") , 0);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_10_pid5, "to10") , 0);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_3_pid5, "ton3") , 0);

		*/

}

