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


int TAMANIO_MEMORIA_PRINCIPAL;
char * MEMORIA_PRINCIPAL;
int TAMANIO_MARCO;
int CANTIDAD_MARCOS;
t_list* tablas_de_paginas;
sem_t mut_tabla_de_paginas;

int main(int argc, char **argv) {
	int carga = cargar_archivo_configuracion();
	int init = inicializar_estructuras();
	return 0;
}

int inicializar_estructuras(){
	TAMANIO_MEMORIA_PRINCIPAL = TAMANIO_MARCO * CANTIDAD_MARCOS;
	crear_memoria_principal();
	tablas_de_paginas= list_create();
	return 0;
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


void cargar_nuevo_programa(int pid, int paginas_requeridas_del_proceso, t_list* tablasDePaginas){

	crear_tabla_de_pagina_de_un_proceso(pid,  paginas_requeridas_del_proceso,  tablasDePaginas );

}

entrada_tabla_de_paginas* inicializar_paginas(int paginas_requeridas){
	entrada_tabla_de_paginas* entradas = malloc(sizeof(entrada_tabla_de_paginas)* paginas_requeridas);
	return entradas;
}


void crear_tabla_de_pagina_de_un_proceso(int pid, int paginas_requeridas_del_proceso, t_list* tablas_de_pags ){

	entrada_tabla_de_paginas* entradas = inicializar_paginas(paginas_requeridas_del_proceso);
	tabla_de_paginas* nueva_tabla = malloc(sizeof(tabla_de_paginas));
	nueva_tabla->pid=pid;
	nueva_tabla->paginas_totales=paginas_requeridas_del_proceso;
	nueva_tabla->entradas=entradas;
	sem_wait(&mut_tabla_de_paginas);
	list_add(tablas_de_paginas, nueva_tabla);
	sem_post(&mut_tabla_de_paginas);
}


