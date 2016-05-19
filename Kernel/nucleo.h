/*
 * nucleo.h

 *
 *  Created on: 8/5/2016
 *      Author: utnso
 */
#ifndef NUCLEO_H_
#define NUCLEO_H_

#include <commons/collections/queue.h>
#include <commons/collections/list.h>

#include <stdio.h>
#include <stdlib.h>
#include <parser/metadata_program.h>
#include <pthread.h>
#include <semaphore.h>

typedef enum {
    NEW = 0,
    READY,
    EXEC,
    BLOCK,
    EXIT
} t_estado;



typedef struct {
	int numero_pagina;
	t_intructions instruccion;
} t_indice_instrucciones_elemento;

typedef struct {

} t_indice_etiqueta;

typedef struct t_PCB {
	int pid;
	t_estado estado;
	int program_counter;
	int paginas_codigo;
	int cantidad_instrucciones;
	t_indice_instrucciones_elemento* indice_instrucciones;
	int cantidad_etiquetas;
	//t_indice_etiqueta TODO: implementar esto
	void* stack_index;
} t_PCB;

static int pid=0;
static sem_t mut_new, mut_ready, mut_block, mut_exit, mut_cpu;
static sem_t cant_new, cant_ready,  cant_block, cant_exit,cant_cpu;

static void *pColaNew, *pColaReady, *pColaExit, *pColaBlock,*pListaCpu;



void *recNew();
void *recReady();
void *recBlock();
void *recExec();
void *recExit();

void Planificacion();

int obtener_cantidad_paginas_programa(t_metadata_program*, int bytes);

#endif /* NUCLEO_H_ */
