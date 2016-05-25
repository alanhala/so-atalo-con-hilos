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
#include "../memoriaPrincipal.h"
#include "../main.h"
#include "CUnit/Basic.h"
#include "test.h"


int correrTest(){

CU_initialize_registry();

      CU_pSuite creacion_de_estructuras = CU_add_suite("Suite creacion de estructuras", NULL, NULL);
	  CU_add_test(creacion_de_estructuras, "cargo config", cargar_archivo_configuracion_umc);
	  CU_add_test(creacion_de_estructuras, "cargo ansisop script", cargar_archivo_ansisop_test);
	  //CU_add_test(creacion_de_estructuras, "dos", crear_50_frames_de_memoria_principal);
	  //rehacer este test a partir del archivo de configuracion



	  CU_pSuite carga_de_programa = CU_add_suite("Suite carga de programa", NULL, NULL);
	  //CU_add_test(carga_de_programa, "uno", cargo_programa_pid_100);
	  CU_add_test(carga_de_programa, "cargo programa sin frames", cargar_programa_sin_asignar_frames);




	  CU_pSuite eliminacion_de_programa = CU_add_suite("Suite eliminacion de programa", NULL, NULL);
	  CU_add_test(eliminacion_de_programa, "eliminar programa sin frames asignados", eliminar_programa_pid_2_sin_ningun_frame_asignado);
	  //TODO HACER TEST PARA CUANDO TENGA ASIGNADO FRAMES
	  //TODO TESTEAR CON SWAP TAMBIEN

	  /*
	  CU_pSuite escritura_de_frame = CU_add_suite("Suite escritura de frame", NULL, NULL);

	  CU_add_test(escritura_de_frame, "uno", asigno_frame_2_a_la_pagina_5);
	  CU_add_test(escritura_de_frame, "dos", no_esta_presente_frame_2_en_pagina_4);
	  CU_add_test(escritura_de_frame, "tres", escribir_hola_en_frame_3_offset_5);
	  CU_add_test(escritura_de_frame, "cuatro", escribir_hola_en_frame_0);
	   */

	  CU_pSuite escritura_de_pagina = CU_add_suite("Suite escritura de pagina en memoria", NULL, NULL);
	  CU_add_test(escritura_de_pagina, "escritura pagina 2 de programa", escribo_pagina_2_de_un_programa);

	  CU_basic_set_mode(CU_BRM_VERBOSE);
	  CU_basic_run_tests();
	  CU_cleanup_registry();

	  return CU_get_error();

}

void cargar_archivo_ansisop_test(){
	//si este test no pasa validar la ruta del archivo
	char * codigo = cargar_ansisop();

	CU_ASSERT_NOT_EQUAL(strlen(codigo) >16, 0);
}

void escribo_pagina_2_de_un_programa(){
	int init = inicializar_estructuras();

	char * buffer = "escritura";
	char * codigo = cargar_ansisop();
	cargar_nuevo_programa(100, 200, codigo);

	int resultado_escritura = escribir_pagina_de_programa(100,  2,  5,  strlen(buffer) +1,  buffer);
	t_tabla_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	int frame_escrito =buscar_frame_de_una_pagina(tabla, 2);
	char * lectura = leer_frame_de_memoria_principal(frame_escrito,5, strlen(buffer)+1);
	CU_ASSERT_EQUAL(strcmp(lectura, buffer), 0);

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
	if(TAMANIO_FRAME != 5)
		incorrecto =1;
	if(MAX_FRAMES_POR_PROCESO != 400)
		incorrecto =1;
	if (CANTIDAD_ENTRADAS_TLB != 300)
		incorrecto =1;
	if (RETARDO != 10000)
		incorrecto =1;
	if(!(ALGORITMO_REEMPLAZO ==99 || ALGORITMO_REEMPLAZO ==1 || ALGORITMO_REEMPLAZO == 2))
			incorrecto =1;

	CU_ASSERT_EQUAL(incorrecto, 0);

}


void cargo_programa_pid_100(){
	//inicializacion_para_test(10, 300);
	int init = inicializar_estructuras();
	crear_swap_mock();


	char * codigo = cargar_ansisop();
	cargar_nuevo_programa(100, 200, codigo);
	t_tabla_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	CU_ASSERT_EQUAL(tabla->pid, 100);
}

void asigno_frame_2_a_la_pagina_5(){
	//inicializacion_para_test(10, 500);
	int init = inicializar_estructuras();
	crear_swap_mock();


	char * codigo = cargar_ansisop();
	cargar_nuevo_programa(100, 200, codigo);
	t_tabla_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	CU_ASSERT_EQUAL(devolver_frame_de_pagina(tabla, 5),2);

}

void no_esta_presente_frame_2_en_pagina_4(){
	//inicializacion_para_test(10, 500);
	int init = inicializar_estructuras();
	crear_swap_mock();


	char * codigo = cargar_ansisop();
	cargar_nuevo_programa(100, 200, codigo);
	t_tabla_de_paginas* tabla= buscar_tabla_de_paginas_de_pid(100);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	CU_ASSERT_NOT_EQUAL(devolver_frame_de_pagina(tabla, 4),2);

}

void escribir_hola_en_frame_3_offset_5(){

	char hola[] = "hola";
	char* p = &hola;
	//inicializacion_para_test(sizeof("hola"), 3);
	inicializar_estructuras();
	crear_swap_mock();

	//escribir_frame_de_memoria_principal(3, 5, sizeof("hola"), p);
	escribir_frame_de_memoria_principal(3, 5, strlen("hola")+1, p);

	//char* datos_escritos = leer_frame_de_memoria_principal(3, 5, sizeof("hola"));
	char* datos_escritos = leer_frame_de_memoria_principal(3, 5, strlen("hola")+1);
	CU_ASSERT_EQUAL(strcmp(datos_escritos, "hola"),0);
}
void escribir_hola_en_frame_0(){

	char hola[] = "hola";
	char* p = &hola;
	//inicializacion_para_test(sizeof("hola"), 6);
	inicializar_estructuras();
	crear_swap_mock();


	escribir_frame_de_memoria_principal(0, 0, sizeof("hola"), p);

	char* datos_escritos = leer_frame_de_memoria_principal(0, 0, sizeof("hola"));
	CU_ASSERT_EQUAL(strcmp(datos_escritos, "hola"),0);
}


void cargar_programa_sin_asignar_frames(){
	//inicializacion_para_test(10, 60);
	inicializar_estructuras();
	crear_swap_mock();


	char * codigo = cargar_ansisop();
	int cargado  = cargar_nuevo_programa(1, 100, codigo);

	int frames_asignado(t_frame *frame) {
					return (frame->asignado == 1);
				}
	int frames_asignados= list_count_satisfying(lista_frames, (void *) frames_asignado);

	CU_ASSERT_EQUAL(frames_asignados, 0);
	CU_ASSERT_EQUAL(cargado, -1); //para ver si se cargo en swap (puede ser mock)

}

void eliminar_programa_pid_2_sin_ningun_frame_asignado(){
	//inicializacion_para_test(10, 90);
	inicializar_estructuras();
	crear_swap_mock();

	char * codigo;
	cargar_nuevo_programa(1, 40, codigo);
	cargar_nuevo_programa(2, 20, codigo);
	cargar_nuevo_programa(3, 10, codigo);
	//busco los valores previos a finalizar el programa
	int viejo_size = list_size(lista_tabla_de_paginas);
	t_tabla_de_paginas * tabla= buscar_tabla_de_paginas_de_pid(2);


	finalizar_programa(2);

	//busco los valores nuevos luego de finalizar el programa
	int nuevo_size = list_size(lista_tabla_de_paginas);



	CU_ASSERT_EQUAL(viejo_size -1, nuevo_size);

}

char * cargar_ansisop(){
	struct FILE *codeF = fopen("/home/utnso/workspace/ansisopScript", "r");

	int cantCar, c;
	cantCar=1;
	while ((c = getc(codeF)) != EOF)
	{
		cantCar++;
	}

	char array[cantCar];



	int i = 0;
	for (i = 0; EOF; i++) {
		c = getc(codeF);
		if (c == EOF) {
			array[i] = '\0';
			break;
		} else {
			array[i] = c;
		}
		}
	return array;
}



