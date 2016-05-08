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
	  CU_add_test(prueba, "cinco", crear_50_frames_de_memoria_principal);
	  //CU_add_test(prueba, "seis", eliminar_tabla_de_pid_2); //TODO TEST INCOMPLETO, hacer un test para eliminar un





	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}



void cargo_programa_pid_100(){
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tablas_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	CU_ASSERT_EQUAL(tabla->pid, 100);
}

void asigno_frame_2_a_la_pagina_5(){
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tablas_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	CU_ASSERT_EQUAL(devolver_frame_de_pagina(tabla, 5),2);

}

void no_esta_presente_frame_2_en_pagina_4(){
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tablas_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	CU_ASSERT_NOT_EQUAL(devolver_frame_de_pagina(tabla, 4),2);

}

void escribir_hola_en_frame_3(){

	char hola[] = "hola";
	char* p = &hola;
	inicializacion_para_test(sizeof("hola"), 3);
	inicializar_estructuras();

	escribir_frame_de_memoria_principal(3, p);

	char* datos_escritos = leer_frame_de_memoria_principal(3);
	CU_ASSERT_EQUAL(strcmp(datos_escritos, "hola"),0);
}
void crear_50_frames_de_memoria_principal(){
	inicializacion_para_test(NULL, 50);
	inicializar_estructuras();
	int cant_frames = list_size(lista_frames_libres);
	CU_ASSERT_EQUAL(cant_frames,50);
}


void eliminar_tabla_de_pid_2(){
	//TODO, analizarlo bien
	inicializacion_para_test(10, 50);
	inicializar_estructuras();

	cargar_nuevo_programa(1, 40);
	cargar_nuevo_programa(2, 20);
	cargar_nuevo_programa(3, 10);
	//busco los valores previos a finalizar el programa
	int viejo_size = list_size(lista_tabla_de_paginas);
	t_tablas_de_paginas * tabla= buscar_tabla_de_paginas_de_pid(2);
	//int frames_que_va_a_liberar = tabla->paginas_totales;
	//int viejo_frames_libres = lista_frames_libres->elements_count;

	finalizar_programa(2);
	//busco los valores nuevos luego de finalizar el programa
	int nuevo_size = list_size(lista_tabla_de_paginas);
	//int nuevo_frames_libres = lista_frames_libres->elements_count;


	CU_ASSERT_EQUAL(viejo_size -1, nuevo_size);
	//CU_ASSERT_EQUAL(viejo_frames_libres + frames_que_va_a_liberar, nuevo_frames_libres);


}
