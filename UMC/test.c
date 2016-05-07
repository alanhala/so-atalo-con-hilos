/*
 * test.c
 *
 *  Created on: 6/5/2016
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
#include "memoriaPrincipal.h"
#include "CUnit/Basic.h"
#include "test.h"


int correrTest(){

CU_initialize_registry();
	  CU_pSuite prueba = CU_add_suite("Suite de prueba", NULL, NULL);
	  CU_add_test(prueba, "uno", cargo_programa_pid_100);
	  CU_add_test(prueba, "dos", asigno_frame_2_a_la_pagina_5);
	  CU_add_test(prueba, "tres", no_esta_presente_frame_2_en_pagina_4);
	  CU_add_test(prueba, "cuarto", escribir_hola_en_frame_3);




	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}



void cargo_programa_pid_100(){
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tablas_de_paginas* tabla= dame_tabla_de_paginas_de_pid(100);
	CU_ASSERT_EQUAL(tabla->pid, 100);
}

void asigno_frame_2_a_la_pagina_5(){
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tablas_de_paginas* tabla= dame_tabla_de_paginas_de_pid(100);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	CU_ASSERT_EQUAL(devolverFrameDePagina(tabla, 5),2);

}

void no_esta_presente_frame_2_en_pagina_4(){
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tablas_de_paginas* tabla= dame_tabla_de_paginas_de_pid(100);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	CU_ASSERT_NOT_EQUAL(devolverFrameDePagina(tabla, 4),2);

}

void escribir_hola_en_frame_3(){

	char hola[] = "hola";
	char* p = &hola;
	inicializacion_para_test(sizeof("hola"), 3);
	inicializar_estructuras();

	escribir_frame_de_memoria_principal(3, p);

	char* datos_escritos = leer_frame_de_memoria_principal(3);
	//char* datos_escritos = malloc(sizeof("hola"));
	//memcpy(datos_escritos, MEMORIA_PRINCIPAL + (3*sizeof("hola")), sizeof("hola"));
	CU_ASSERT_EQUAL(strcmp(datos_escritos, "hola"),0);


}
