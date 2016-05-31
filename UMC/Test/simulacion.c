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

void simulacion_1();
void simulacion_simple();
int simulaciones(){

CU_initialize_registry();

      CU_pSuite simulaciones = CU_add_suite("Suite simulaciones", NULL, NULL);
	  CU_add_test(simulaciones, "simulacion_1", simulacion_1);
	  //CU_add_test(simulaciones, "simulacion_simple", simulacion_simple);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();



	  return CU_get_error();

}



void simulacion_1(){
	inicializar_estructuras();
	set_algoritmo_reemplazo("test");
	crear_swap_mock();

//	int swap_socket = create_client_socket_descriptor("192.168.0.33", "6000");
	//set_socket_descriptor(swap_socket);
	char * codigo = "pag00pag01pag02pag03pag04pag05pag06pag07pag08pag09pag10pag11pag12pag13pag14pag15pag16pag17pag18" ;
	int tamanio_codigo=	strlen(codigo); //no agrego el /0

	int paginas_necesarias = (tamanio_codigo/TAMANIO_FRAME);
	set_max_frames_por_proceso(paginas_necesarias - 10);
	cargar_nuevo_programa(0, paginas_necesarias, codigo);
	cargar_nuevo_programa(5, paginas_necesarias, codigo);

	int lecturas=0;
	int resultado_lectura = 0;
	for(lecturas; lecturas < 16; lecturas ++){
		char * lo_que_leo =  leer_pagina_de_programa(0, lecturas, 0, TAMANIO_FRAME);
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



	int lecturas_pid5=0;
	int resultado_lectura_pid5 = 0;
	for(lecturas_pid5; lecturas_pid5 < 16; lecturas_pid5 ++){
		char * lo_que_leo_pid5 =  leer_pagina_de_programa(5, lecturas_pid5, 0, TAMANIO_FRAME);
		if (!strcmp(lo_que_leo_pid5, "~/-1"))
			resultado_lectura = -1;
	}


	char * lectura_una_pagina_0_pid5 =  leer_pagina_de_programa(5, 0, 0, TAMANIO_FRAME);
	char * lectura_una_pagina_17_pid5 =  leer_pagina_de_programa(5, 17, 0, TAMANIO_FRAME);
	char * lectura_una_pagina_13_pid5 =  leer_pagina_de_programa(5, 13, 0, TAMANIO_FRAME);

	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_0_pid5, "pag00") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_17_pid5, "pag17") , 0);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_13_pid5, "pag13") , 0);

	escribir_pagina_de_programa(5, 13, 0, TAMANIO_FRAME, "tony");
	lectura_una_pagina_13_pid5 =  leer_pagina_de_programa(5, 13, 0, TAMANIO_FRAME);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_13_pid5, "tony") , 0);

}



void simulacion_simple(){
	inicializar_estructuras();

	//crear_swap_mock();
	//printf("%s", SWAPIP); //TODO TENGO ERROR ACA
	int swap_socket = create_client_socket_descriptor("192.168.0.33", "6000");
	set_socket_descriptor(swap_socket);
	char * codigo = "pag00pag01pag02pag03pag04pag05pag06pag07pag08pag09" ;
	//int tamanio_codigo=	strlen(codigo); //no agrego el /0

	int paginas_necesarias = 12;
	set_max_frames_por_proceso(paginas_necesarias - 5);
	cargar_nuevo_programa(0, paginas_necesarias, codigo);


	int lecturas=0;
	int resultado_lectura = 0;
	char * lo_que_leo =  leer_pagina_de_programa(0, 6, 0, TAMANIO_FRAME);
	char * directo_de_memoria = leer_frame_de_memoria_principal(5,0, TAMANIO_FRAME);

	send(SWAP_SOCKET_DESCRIPTOR, "4", strlen("1"), 0);
	CU_ASSERT_EQUAL(strcmp(directo_de_memoria, "pag05") , 0);
	CU_ASSERT_EQUAL(strcmp(lo_que_leo, "pag05") , 0);

	//for(lecturas; lecturas < 16; lecturas ++){
	//	char * lo_que_leo =  leer_pagina_de_programa(0, lecturas, 0, TAMANIO_FRAME);
	//	if (!strcmp(lo_que_leo, "~/-1"))
	//		resultado_lectura = -1;
	//}


//	CU_ASSERT_EQUAL(resultado_lectura , 0);

	//char * lectura_una_pagina =  leer_pagina_de_programa(0, 0, 0, TAMANIO_FRAME);
	//char * lectura_una_pagina_5 =  leer_pagina_de_programa(0, 2, 0, TAMANIO_FRAME);
	//char * lectura_una_pagina_18 =  leer_pagina_de_programa(0, 18, 0, TAMANIO_FRAME);

	//CU_ASSERT_EQUAL(strcmp(lectura_una_pagina, "pag00") , 0);
	//CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_5, "pag02") , 0);
	//CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_18, "pag18") , 0);

	/*
	escribir_pagina_de_programa(0, 17, 0, TAMANIO_FRAME, "piatti");
	char * lectura_una_pagina_17 =  leer_pagina_de_programa(0, 17, 0, TAMANIO_FRAME);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_17, "piatt") , 0);

	escribir_pagina_de_programa(5, 5, 0, TAMANIO_FRAME, "piatti");
	char * lectura_una_pagina_5_pid5 =  leer_pagina_de_programa(5, 5, 0, TAMANIO_FRAME);
	CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_5_pid5, "piatt") , 0);
	*/
}
