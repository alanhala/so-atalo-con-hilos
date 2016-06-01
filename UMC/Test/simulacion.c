/*
 * simulacion.c
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
#include "simulacion.h"


void print_memoria_principal() {
	int frame = 0;
	for (frame; 200; frame++) {
		t_frame* f = list_get(lista_frames, frame);
		if (f->asignado == 1) {
			char* lectura = leer_frame_de_memoria_principal(frame, 0,
					TAMANIO_FRAME);
			printf("%s", lectura);
		}
	}
}

void simulacion_1();
void simulacion_2();
void simulacion_3();
void simulacion_alternando_programas();

void simulacion_simple();
int simulaciones(){

CU_initialize_registry();

      CU_pSuite simulaciones = CU_add_suite("Suite simulaciones", NULL, NULL);
      CU_add_test(simulaciones, "simulacion_1", simulacion_1);
	  CU_add_test(simulaciones, "simulacion_2", simulacion_2);
      CU_add_test(simulaciones, "simulacion_3", simulacion_3);
      CU_add_test(simulaciones, "simulacion_alternando_programas", simulacion_alternando_programas);
	  //CU_add_test(simulaciones, "simulacion_simple", simulacion_simple);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();



	  return CU_get_error();

}



void simulacion_2(){
	inicializar_estructuras();
	set_algoritmo_reemplazo("ClockM");

	crear_swap_mock();

	//int swap_socket = create_client_socket_descriptor("localhost", "6000");
	//set_socket_descriptor(swap_socket);
	char * codigo_pid0= "0pg000pg010pg020pg030pg040pg050pg060pg070pg080pg090pg100pg110pg120pg130pg14";
	char * codigo_pid5 = "5pg005pg015pg025pg035pg045pg055pg065pg075pg085pg095pg105pg115pg125pg135pg14";


	int paginas_necesarias = (30);
	set_max_frames_por_proceso(5);
	cargar_nuevo_programa(0, paginas_necesarias, codigo_pid0);
	cargar_nuevo_programa(5, paginas_necesarias, codigo_pid5);

	int lecturas=0;
	int resultado_lectura = 0;
	for(lecturas; lecturas < 10; lecturas ++){
		char * lo_que_leo =  leer_pagina_de_programa(0, lecturas, 0, TAMANIO_FRAME);
		if (!strcmp(lo_que_leo, "~/-1"))
			resultado_lectura = -1;
	}

	int lecturas_pid5=0;
	int resultado_lectura_pid5 = 0;
	for(lecturas_pid5; lecturas_pid5 < 10; lecturas_pid5 ++){
		char * lo_que_leo_pid5 =  leer_pagina_de_programa(5, lecturas_pid5, 0, TAMANIO_FRAME);
		if (!strcmp(lo_que_leo_pid5, "~/-1"))
			resultado_lectura_pid5 = -1;
	}
	CU_ASSERT_EQUAL(resultado_lectura , 0);
	CU_ASSERT_EQUAL(resultado_lectura_pid5 , 0);
	//print_memoria_principal();
}

void simulacion_1(){
	inicializar_estructuras();
	set_algoritmo_reemplazo("ClockM");
//	set_algoritmo_reemplazo("Clock");
	crear_swap_mock();

	//int swap_socket = create_client_socket_descriptor("localhost", "6000");
	//set_socket_descriptor(swap_socket);
	char * codigo = "pag00pag01pag02pag03pag04pag05pag06pag07pag08pag09pag10pag11pag12pag13pag14pag15pag16pag17pag18" ;
	int tamanio_codigo=	strlen(codigo); //no agrego el /0

	//int paginas_necesarias = (tamanio_codigo/TAMANIO_FRAME);
	int paginas_necesarias = 30;
	set_max_frames_por_proceso(5);
	cargar_nuevo_programa(0, paginas_necesarias, codigo);
	cargar_nuevo_programa(5, paginas_necesarias, codigo);

	int lecturas=0;
	int resultado_lectura = 0;
	for(lecturas; lecturas < 16; lecturas ++){
		char * lo_que_leo =  leer_pagina_de_programa(0, lecturas, 0, TAMANIO_FRAME);
		printf("%s\n", lo_que_leo);
		if (!strcmp(lo_que_leo, "~/-1"))
			resultado_lectura = -1;
	}


	CU_ASSERT_EQUAL(resultado_lectura , 0);

	char * lectura_una_pagina =  leer_pagina_de_programa(0, 0, 0, TAMANIO_FRAME);
	char * lectura_una_pagina_2 =  leer_pagina_de_programa(0, 2, 0, TAMANIO_FRAME);
	char * lectura_una_pagina_18 =  leer_pagina_de_programa(0, 18, 0, TAMANIO_FRAME);

	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina, "pag00") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_2, "pag02") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_18, "pag18") , 0);


	escribir_pagina_de_programa(0, 17, 0, TAMANIO_FRAME, "piatti");
	char * lectura_una_pagina_17 =  leer_pagina_de_programa(0, 17, 0, TAMANIO_FRAME);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_17, "piatt") , 0);

	escribir_pagina_de_programa(5, 5, 0, TAMANIO_FRAME, "piatti");
	char * lectura_una_pagina_5_pid5 =  leer_pagina_de_programa(5, 5, 0, TAMANIO_FRAME);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_5_pid5, "piatt") , 0);


	printf("comienzo lectura del pid 5");
	int lecturas_pid5=0;
	int resultado_lectura_pid5 = 0;
	for(lecturas_pid5; lecturas_pid5 < 16; lecturas_pid5 ++){
		char * lo_que_leo_pid5 =  leer_pagina_de_programa(5, lecturas_pid5, 0, TAMANIO_FRAME);
		printf("%s\n", lo_que_leo_pid5);
		if (!strcmp(lo_que_leo_pid5, "~/-1"))
			resultado_lectura_pid5 = -1;
	}

	CU_ASSERT_EQUAL(resultado_lectura_pid5 , 0);

	char * lectura_una_pagina_0_pid5 =  leer_pagina_de_programa(5, 0, 0, TAMANIO_FRAME);
	char * lectura_una_pagina_17_pid5 =  leer_pagina_de_programa(5, 17, 0, TAMANIO_FRAME);
	char * lectura_una_pagina_13_pid5 =  leer_pagina_de_programa(5, 13, 0, TAMANIO_FRAME);

	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_0_pid5, "pag00") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_17_pid5, "pag17") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_13_pid5, "pag13") , 0);

	escribir_pagina_de_programa(5, 13, 0, TAMANIO_FRAME, "tony");
	lectura_una_pagina_13_pid5 =  leer_pagina_de_programa(5, 13, 0, TAMANIO_FRAME);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_13_pid5, "tony") , 0);


	//print_memoria_principal();
}


void simulacion_3(){

	inicializar_estructuras();
	set_algoritmo_reemplazo("ClockM");
	crear_swap_mock();

	//	int swap_socket = create_client_socket_descriptor("localhost", "6000");
		//set_socket_descriptor(swap_socket);
		char * codigo_pid0= "0pg000pg010pg020pg030pg040pg050pg060pg070pg080pg090pg100pg110pg120pg130pg14";
		char * codigo_pid5 = "5pg005pg015pg025pg035pg045pg055pg065pg075pg085pg095pg105pg115pg125pg135pg14";


		int paginas_necesarias = (30);
		set_max_frames_por_proceso(5);
		cargar_nuevo_programa(0, paginas_necesarias, codigo_pid0);
		cargar_nuevo_programa(5, paginas_necesarias, codigo_pid5);


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

		//print_memoria_principal();


}


void simulacion_alternando_programas(){

	inicializar_estructuras();
	//set_algoritmo_reemplazo("test");
	set_algoritmo_reemplazo("ClockM");

	crear_swap_mock();

	//int swap_socket = create_client_socket_descriptor("localhost", "6000");
	//set_socket_descriptor(swap_socket);
	char * codigo_pid0= "0pg000pg010pg020pg030pg040pg050pg060pg070pg080pg090pg100pg110pg120pg130pg14";
	char * codigo_pid5 = "5pg005pg015pg025pg035pg045pg055pg065pg075pg085pg095pg105pg115pg125pg135pg14";
	char * codigo_pid3 = "3pg003pg013pg023pg033pg043pg053pg063pg073pg083pg093pg103pg113pg123pg133pg14";


	int paginas_necesarias = (30);
	set_max_frames_por_proceso(5);
	cargar_nuevo_programa(0, paginas_necesarias, codigo_pid0);
	cargar_nuevo_programa(5, paginas_necesarias, codigo_pid5);
	cargar_nuevo_programa(3, paginas_necesarias, codigo_pid3);


	escribir_pagina_de_programa(0, 3, 0, TAMANIO_FRAME, "piatt");
	escribir_pagina_de_programa(3, 3, 0, TAMANIO_FRAME, "piatt");
	escribir_pagina_de_programa(5, 3, 0, TAMANIO_FRAME, "piatt");
	char * lectura_una_pagina_3_pid0 =  leer_pagina_de_programa(0, 3, 0, TAMANIO_FRAME);
	char * lectura_una_pagina_3_pid3 =  leer_pagina_de_programa(3, 3, 0, TAMANIO_FRAME);
	char * lectura_una_pagina_3_pid5 =  leer_pagina_de_programa(5, 3, 0, TAMANIO_FRAME);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_3_pid0, "piatt") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_3_pid3, "piatt") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_3_pid5, "piatt") , 0);




	char * lectura_pagina_6_pid0 =  leer_pagina_de_programa(0, 6, 0, TAMANIO_FRAME);
	char * lectura_pagina_8_pid3 =  leer_pagina_de_programa(3, 8, 0, 3);
	char * lectura_pagina_11_pid5 =  leer_pagina_de_programa(5, 11, 0, TAMANIO_FRAME);

	CU_ASSERT_EQUAL(strcmp(lectura_pagina_6_pid0, "0pg06") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_pagina_8_pid3, "3pg") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_pagina_11_pid5, "5pg11") , 0);



	//print_memoria_principal();


}




void simulacion_simple(){
	inicializar_estructuras();

	crear_swap_mock();
	//printf("%s", SWAPIP); //TODO TENGO ERROR ACA
	//int swap_socket = create_client_socket_descriptor("192.168.0.33", "6000");
	//set_socket_descriptor(swap_socket);
	char * codigo = "pag00pag01pag02pag03pag04pag05pag06pag07pag08pag09" ;
	//int tamanio_codigo=	strlen(codigo); //no agrego el /0

	int paginas_necesarias = 12;
	set_max_frames_por_proceso(paginas_necesarias - 5);
	cargar_nuevo_programa(0, paginas_necesarias, codigo);


	int lecturas=0;
	int resultado_lectura = 0;
	char * lo_que_leo =  leer_pagina_de_programa(0, 6, 0, TAMANIO_FRAME);


	send(SWAP_SOCKET_DESCRIPTOR, "4", strlen("1"), 0);

	CU_ASSERT_EQUAL(strcmp(lo_que_leo, "pag06") , 0);


}
