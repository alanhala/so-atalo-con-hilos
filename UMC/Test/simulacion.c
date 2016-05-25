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

int simulaciones(){

CU_initialize_registry();

      CU_pSuite simulaciones = CU_add_suite("Suite simulaciones", NULL, NULL);
	  CU_add_test(simulaciones, "simulacion_1", simulacion_1);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();



	  return CU_get_error();

}



void simulacion_1(){
	//voy a cargar 1 programas, las paginas necesarias van a ser 5 mas que el tamanio del codigo ansisop
	//voy a leer el contenido de la pagina 0 a las necesarias.
	inicializar_estructuras();
	crear_swap_mock();
	char * codigo = "pag01pag02pag03pag04pag05pag06pag07pag08pag09pag10pag11pag12pag13pag14pag15pag16pag17pag18" ;
	int tamanio_codigo=	strlen(codigo); //no agrego el /0

	int paginas_necesarias = (tamanio_codigo/TAMANIO_FRAME);
	set_max_frames_por_proceso(paginas_necesarias - 10);
	cargar_nuevo_programa(0, paginas_necesarias, codigo);

	int lecturas=0;
	int resultado_lectura = 0;
	for(lecturas; lecturas < 16; lecturas ++){
		char * lo_que_leo =  leer_pagina_de_programa(0, lecturas, 0, TAMANIO_FRAME);
		if (!strcmp(lo_que_leo, "~/-1"))
			resultado_lectura = -1;
	}

	CU_ASSERT_EQUAL(resultado_lectura , 0);

}
