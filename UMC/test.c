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
#include "main.h"
#include "CUnit/Basic.h"
#include "test.h"


int correrTest(){

CU_initialize_registry();

      CU_pSuite creacion_de_estructuras = CU_add_suite("Suite creacion de estructuras", NULL, NULL);
	  CU_add_test(creacion_de_estructuras, "uno", cargar_archivo_configuracion_umc);
	  //CU_add_test(creacion_de_estructuras, "dos", crear_50_frames_de_memoria_principal);
	  //rehacer este test a partir del archivo de configuracion



	  CU_pSuite carga_de_programa = CU_add_suite("Suite carga de programa", NULL, NULL);
	  CU_add_test(carga_de_programa, "uno", cargo_programa_pid_100);
	  CU_add_test(carga_de_programa, "dos", cargar_programa_asignando_20_frames);




	  CU_pSuite eliminacion_de_programa = CU_add_suite("Suite eliminacion de programa", NULL, NULL);
	  CU_add_test(eliminacion_de_programa, "uno", eliminar_programa_pid_2);


	  CU_pSuite escritura_de_frame = CU_add_suite("Suite escritura de frame", NULL, NULL);

	  CU_add_test(escritura_de_frame, "uno", asigno_frame_2_a_la_pagina_5);
	  CU_add_test(escritura_de_frame, "dos", no_esta_presente_frame_2_en_pagina_4);
	  CU_add_test(escritura_de_frame, "tres", escribir_hola_en_frame_3_offset_5);
	  CU_add_test(escritura_de_frame, "cuatro", escribir_hola_en_frame_0);


	  CU_pSuite escritura_de_pagina = CU_add_suite("Suite escritura de pagina en memoria", NULL, NULL);

	  CU_add_test(escritura_de_pagina, "uno", escribo_pagina_2_de_un_programa);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}

void escribo_pagina_2_de_un_programa(){
	int init = inicializar_estructuras();

	//int resultado_escritura = escribir_pagina_de_programa(int pid, int pagina, int offset, int size, char * buffer);

}

void cargar_archivo_configuracion_umc(){
	// si este test no pasa, para checkear rapido que atributo del archivo no se cargo correctamente
	// poner un breakpoint despues de cada if y ver que condicion no se cumplio
	int result = cargar_configuracion();
	int incorrecto =0;
	if(LISTENPORT !=21000)
		incorrecto =1;
	if (!strncmp(SWAPIP,"localhost", sizeof("localhost")))
		incorrecto =1;
	if(SWAPPORT != 8000)
		incorrecto =1;
	if(CANTIDAD_FRAMES !=4000)
		incorrecto =1;
	if(TAMANIO_FRAME != 50)
		incorrecto =1;
	if(MAX_FRAMES_POR_PROCESO != 400)
		incorrecto =1;
	if (CANTIDAD_ENTRADAS_TLB != 300)
		incorrecto =1;
	if (RETARDO != 10000)
		incorrecto =1;

	CU_ASSERT_EQUAL(incorrecto, 0);

}


void cargo_programa_pid_100(){
	//inicializacion_para_test(10, 300);
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tabla_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	CU_ASSERT_EQUAL(tabla->pid, 100);
}

void asigno_frame_2_a_la_pagina_5(){
	//inicializacion_para_test(10, 500);
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tabla_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	CU_ASSERT_EQUAL(devolver_frame_de_pagina(tabla, 5),2);

}

void no_esta_presente_frame_2_en_pagina_4(){
	//inicializacion_para_test(10, 500);
	int init = inicializar_estructuras();
	cargar_nuevo_programa(100, 200);
	t_tabla_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	CU_ASSERT_NOT_EQUAL(devolver_frame_de_pagina(tabla, 4),2);

}

void escribir_hola_en_frame_3_offset_5(){

	char hola[] = "hola";
	char* p = &hola;
	//inicializacion_para_test(sizeof("hola"), 3);
	inicializar_estructuras();

	escribir_frame_de_memoria_principal(3, 5, sizeof("hola"), p);

	char* datos_escritos = leer_frame_de_memoria_principal(3, 5, sizeof("hola"));
	CU_ASSERT_EQUAL(strcmp(datos_escritos, "hola"),0);
}
void escribir_hola_en_frame_0(){

	char hola[] = "hola";
	char* p = &hola;
	//inicializacion_para_test(sizeof("hola"), 6);
	inicializar_estructuras();

	escribir_frame_de_memoria_principal(0, 0, sizeof("hola"), p);

	char* datos_escritos = leer_frame_de_memoria_principal(0, 0, sizeof("hola"));
	CU_ASSERT_EQUAL(strcmp(datos_escritos, "hola"),0);
}


void cargar_programa_asignando_20_frames(){
	//inicializacion_para_test(10, 60);
	inicializar_estructuras();

	cargar_nuevo_programa(1, 20);

	int frames_asignado(t_frame *frame) {
					return (frame->asignado == 1);
				}
	int frames_asignados= list_count_satisfying(lista_frames, (void *) frames_asignado);

	CU_ASSERT_EQUAL(20, frames_asignados);

}

void eliminar_programa_pid_2(){
	//inicializacion_para_test(10, 90);
	inicializar_estructuras();

	cargar_nuevo_programa(1, 40);
	cargar_nuevo_programa(2, 20);
	cargar_nuevo_programa(3, 10);
	//busco los valores previos a finalizar el programa
	int viejo_size = list_size(lista_tabla_de_paginas);
	t_tabla_de_paginas * tabla= buscar_tabla_de_paginas_de_pid(2);

	int frames_que_va_a_liberar = tabla->paginas_totales;

	int frames_no_asignados(t_frame *frame) {
				return (frame->asignado == 0);
			}
	int viejo_frames_libres = list_count_satisfying(lista_frames, (void *) frames_no_asignados);

	finalizar_programa(2);

	//busco los valores nuevos luego de finalizar el programa
	int nuevo_size = list_size(lista_tabla_de_paginas);
	int nuevo_frames_libres = list_count_satisfying(lista_frames, (void *)frames_no_asignados);


	CU_ASSERT_EQUAL(viejo_size -1, nuevo_size);
	CU_ASSERT_EQUAL(viejo_frames_libres + frames_que_va_a_liberar, nuevo_frames_libres);
}
