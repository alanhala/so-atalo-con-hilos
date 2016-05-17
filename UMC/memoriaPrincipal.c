/*
 * memoriaPrincipal.c
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
#include "socket.h"
#include <commons/collections/list.h>
#include <semaphore.h>
#include "memoriaPrincipal.h"

int inicializar_estructuras() {
	inicializar_semaforos();
	TAMANIO_MEMORIA_PRINCIPAL = TAMANIO_FRAME * CANTIDAD_FRAMES;
	crear_memoria_principal();
	lista_tabla_de_paginas = list_create();
	crear_lista_frames();
	return 0;
}

void inicializar_semaforos() {
	sem_init(&mut_tabla_de_paginas, 0, 1);
	sem_init(&mut_lista_frames, 0, 1);

}
int cargar_archivo_configuracion() {
	//TODO cargar archivo de configuracion y a partir de eso setear los valores
	TAMANIO_FRAME = 50;
	CANTIDAD_FRAMES = 2000;
	return 0;
}

void crear_memoria_principal() {
	char* datos = malloc(TAMANIO_MEMORIA_PRINCIPAL);
	memset(datos, '\0', TAMANIO_MEMORIA_PRINCIPAL);
	MEMORIA_PRINCIPAL = datos;
}

//solo para test
void inicializacion_para_test(int tamanio_frame, int cantidad_frame) {
	CANTIDAD_FRAMES = cantidad_frame;
	TAMANIO_FRAME = tamanio_frame;
}

void liberar_memoria_principal() {
	free(MEMORIA_PRINCIPAL);
}

void cargar_nuevo_programa(int pid, int paginas_requeridas_del_proceso) {
	//TODO IMPORTANTE validar si hay espacio en memoria principal y/o swap. PREGUNTAR el criterio a utilizar


	t_tablas_de_paginas * tabla = crear_tabla_de_pagina_de_un_proceso(pid, paginas_requeridas_del_proceso);
	int pagina=0;
	for( 0 ; pagina < tabla->paginas_totales; pagina ++){
		int frame_libre = buscar_frame_libre();
		asignar_frame_a_una_pagina(tabla, frame_libre, pagina);
	}

}
int buscar_frame_libre(){
	int frame_libre(t_frame *frame) {
			return (frame->asignado == 0);
		}

	t_frame* frame_encontrado = list_find(lista_frames,	(void*) frame_libre);

	return frame_encontrado->frame;
}

void finalizar_programa(int pid){
	//TODO avisarle a swap que finalice el programa

	t_tablas_de_paginas* tabla = buscar_tabla_de_paginas_de_pid(pid);

	int pagina=0;
	for(0; pagina < tabla->paginas_totales ; pagina++ ){
		int frame = devolver_frame_de_pagina(tabla, pagina);
		marcar_frame_como_libre(frame);
	}

	int pid_iguales(t_tablas_de_paginas *tabla) {
			return (tabla->pid == pid);
		}

	list_remove_by_condition(lista_tabla_de_paginas, pid_iguales);


}

t_entrada_tabla_de_paginas* inicializar_paginas(int paginas_requeridas_del_proceso) {
	t_entrada_tabla_de_paginas* entradas = malloc(sizeof(t_entrada_tabla_de_paginas)* paginas_requeridas_del_proceso);
	return entradas;
}

t_tablas_de_paginas * crear_tabla_de_pagina_de_un_proceso(int pid, int paginas_requeridas_del_proceso) {

	t_entrada_tabla_de_paginas* entradas = inicializar_paginas(paginas_requeridas_del_proceso);
	t_tablas_de_paginas* nueva_tabla = malloc(sizeof(t_tablas_de_paginas));
	nueva_tabla->pid = pid;
	nueva_tabla->paginas_totales = paginas_requeridas_del_proceso;
	nueva_tabla->entradas = entradas;
	sem_wait(&mut_tabla_de_paginas);
	list_add(lista_tabla_de_paginas, nueva_tabla);
	sem_post(&mut_tabla_de_paginas);

	return nueva_tabla;
}

void asignar_frame_a_una_pagina(t_tablas_de_paginas* tabla, int frame_a_asignar,
		int pagina) {


	tabla->entradas[pagina].frame = frame_a_asignar;

	int frames_iguales(t_frame *frame) {
				return (frame->frame == frame_a_asignar);
			}

	t_frame * frame_a_modificar = list_find(lista_frames, (void *)frames_iguales);
	frame_a_modificar->asignado = 1;

}

int devolver_frame_de_pagina(t_tablas_de_paginas* tabla, int pagina) {
	return tabla->entradas[pagina].frame;

}

char* leer_frame_de_memoria_principal(int frame) {

	char* datos = malloc(TAMANIO_FRAME);
	memcpy(datos, MEMORIA_PRINCIPAL + (frame * TAMANIO_FRAME), TAMANIO_FRAME);
	return datos;
}

void escribir_frame_de_memoria_principal(int frame, char* datos) {
	//TODO esta funcion solo debe ser llamada si datos =< tamanio frame.
	//TODO la funciona que la llama deberia cortar los datos en un array de datos de tamanio de frame

	//TODO hacer test/analizar que pasa si los datos son menores a un frame(para que no queden bytes feos

	memcpy(MEMORIA_PRINCIPAL + (frame * TAMANIO_FRAME), datos, TAMANIO_FRAME);
}

t_tablas_de_paginas* buscar_tabla_de_paginas_de_pid(int pid_buscado) {
	sem_wait(&mut_tabla_de_paginas);

	int pid_iguales(t_tablas_de_paginas *tabla) {
		return (tabla->pid == pid_buscado);
	}

	t_tablas_de_paginas* tabla = list_find(lista_tabla_de_paginas,
			(void*) pid_iguales);
	sem_post(&mut_tabla_de_paginas);
	return tabla;

}

void crear_lista_frames() {
	sem_wait(&mut_lista_frames);
	lista_frames = list_create();
	int i = 0;
	while (i < CANTIDAD_FRAMES) {
		agregar_frame_a_lista_de_frames(i);
		i++;
	}
	sem_post(&mut_lista_frames);
}

void agregar_frame_a_lista_de_frames(int numero_de_frame) {
	t_frame* nuevo_frame = malloc(sizeof(t_frame));
	nuevo_frame->frame = numero_de_frame;
	nuevo_frame->asignado = 0;
	list_add(lista_frames, nuevo_frame);
}


void marcar_frame_como_libre(int numero_de_frame){
	int frames_iguales(t_frame *frame) {
					return (frame->frame == numero_de_frame);
				}
	t_frame * frame_a_modificar = list_find(lista_frames, (void *)frames_iguales);
	frame_a_modificar->asignado = 0;
}





// CONFIGURACION

void set_cantidad_entradas_tlb(int entradas){
	CANTIDAD_ENTRADAS_TLB =entradas;
}
void set_max_frames_por_proceso(int cantidad){
	MAX_FRAMES_POR_PROCESO = cantidad;
}
void set_cantidad_frames(int cantidad_frames){
	CANTIDAD_FRAMES = cantidad_frames;
}
void set_tamanio_frame(int tamanio_frame){
	TAMANIO_FRAME= tamanio_frame;
}
void set_retardo(int retardo){
	RETARDO = retardo;
}
