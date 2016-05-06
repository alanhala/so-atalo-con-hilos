/*
 * umc_play_main.c
 *
 *  Created on: 6/5/2016
 *      Author: utnso
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "socket.h"
#include <commons/collections/list.h>
#include <semaphore.h>
#include "umc_play_main.h"
#include "CUnit/Basic.h"


int TAMANIO_MEMORIA_PRINCIPAL;
char * MEMORIA_PRINCIPAL;
int TAMANIO_MARCO;
int CANTIDAD_MARCOS;
t_list* lista_tabla_de_paginas;
sem_t mut_tabla_de_paginas;

int main(int argc, char **argv) {
	//zona de test
	int init = inicializar_estructuras();
	printf("inicie las estructuras \n");
	fflush(stdout);
	cargar_nuevo_programa(100, 200);
	printf("creo un programa con pid 100 \n");
	fflush(stdout);
	cargar_nuevo_programa(200, 200);
	printf("creo un programa con pid 200 \n");
	fflush(stdout);

	t_tablas_de_paginas* tabla= dame_tabla_de_paginas_de_pid(100);
	//TODO ANALIZAR LO QUE DEVUELVE CUANDO NO ENCUENTRA LA PAGINA PORQUE NO AVANZO CON EL PROGRAMA
	printf("busco la tabla de paginas de PID:%d\n", tabla->pid);
	fflush(stdout);
	asignar_frame_a_una_pagina(tabla, 2, 5);
	printf("asigno el frame 2 a la pagina 5 del PID:%d\n", tabla->pid);
	fflush(stdout);
	int frame_de_pag_5 =devolverFrameDePagina(tabla, 5);
	if (frame_de_pag_5 == 2){
		printf("asignacion correcta\n");
		fflush(stdout);
	}else
	{
		printf("asignacion incorrecta\n");
		fflush(stdout);
	}
	int frame_de_pag_4 =devolverFrameDePagina(tabla, 4);
	if (frame_de_pag_4 == 2){
			printf("assertfalse incorrecto\n");
			fflush(stdout);
		}else
		{
			printf("assertfalse correcto\n");
			fflush(stdout);
		}
	// fin zona de test

	while(1){

	}


	//int carga = cargar_archivo_configuracion();
	//int init = inicializar_estructuras();
	return 0;
}

int inicializar_estructuras(){
	inicializar_semaforos();
	TAMANIO_MEMORIA_PRINCIPAL = TAMANIO_MARCO * CANTIDAD_MARCOS;
	crear_memoria_principal();
	lista_tabla_de_paginas= list_create();
	return 0;
}

void inicializar_semaforos(){
	sem_init(&mut_tabla_de_paginas, 0, 1);

}
int cargar_archivo_configuracion(){
	//TODO cargar archivo de configuracion y a partir de eso setear los valores
	TAMANIO_MARCO=50;
	CANTIDAD_MARCOS=2000;
	return 0;
}

void crear_memoria_principal(){
	char* datos = malloc(TAMANIO_MEMORIA_PRINCIPAL);
	memset(datos, '\0', TAMANIO_MEMORIA_PRINCIPAL);
	MEMORIA_PRINCIPAL=datos;
}

void liberar_memoria_principal()
{
	free(MEMORIA_PRINCIPAL);
}


void cargar_nuevo_programa(int pid, int paginas_requeridas_del_proceso){
	//TODO validar si hay espacio en memoria principal
	crear_tabla_de_pagina_de_un_proceso(pid,  paginas_requeridas_del_proceso );

}

t_entrada_tabla_de_paginas* inicializar_paginas(int paginas_requeridas_del_proceso){
	t_entrada_tabla_de_paginas* entradas = malloc(sizeof(t_entrada_tabla_de_paginas)* paginas_requeridas_del_proceso);
	return entradas;
}


void crear_tabla_de_pagina_de_un_proceso(int pid, int paginas_requeridas_del_proceso){

	t_entrada_tabla_de_paginas* entradas = inicializar_paginas(paginas_requeridas_del_proceso);
	t_tablas_de_paginas* nueva_tabla = malloc(sizeof(t_tablas_de_paginas));
	nueva_tabla->pid=pid;
	nueva_tabla->paginas_totales=paginas_requeridas_del_proceso;
	nueva_tabla->entradas=entradas;
	sem_wait(&mut_tabla_de_paginas);
	list_add(lista_tabla_de_paginas, nueva_tabla);
	sem_post(&mut_tabla_de_paginas);
}


void asignar_frame_a_una_pagina(t_tablas_de_paginas* tabla, int frame_a_asignar, int pagina){
	tabla->entradas[pagina].frame=frame_a_asignar;
}

int devolverFrameDePagina(t_tablas_de_paginas* tabla, int pagina)
{
	return tabla->entradas[pagina].frame;

}




t_tablas_de_paginas* dame_tabla_de_paginas_de_pid(int pid_buscado){
	sem_wait(&mut_tabla_de_paginas);

	int pid_iguales(t_tablas_de_paginas *tabla) {
						return (tabla->pid == pid_buscado);}


	t_tablas_de_paginas* tabla = list_find(lista_tabla_de_paginas, (void*) pid_iguales);
	sem_post(&mut_tabla_de_paginas);
	return tabla;

}


