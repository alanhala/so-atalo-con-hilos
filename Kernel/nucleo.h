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

//valores de configuracion
int puertoCPU,puertoCON,puertoUMC;
char arrUMCip[10];

sem_t mut_new, mut_ready, mut_block, mut_exit, mut_ejecucion, mut_cpu_disponibles;
sem_t cant_new, cant_ready,
        cant_block, cant_exit, cant_ejecucion, cant_cpu_disponibles;

t_queue *estado_new, *estado_ready,
            *estado_exit, *estado_block,*estado_ejecucion, *cola_cpu_disponibles;
int pid;


int UMC_SOCKET_DESCRIPTOR;
void set_umc_socket_descriptor(int socket);
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

typedef struct {
	int pid;
	t_estado estado;
	char * codigo_programa;
	int program_counter;
	int paginas_codigo;
	int cantidad_instrucciones;
	t_indice_instrucciones_elemento* indice_instrucciones;
	int cantidad_etiquetas;
	//t_indice_etiqueta TODO: implementar esto
	void* stack_index;
} t_PCB;
typedef enum {
    CPU_IDLE = 0, //compu nueva o cpu sin trabajar
    fin_quantum, //fin quantum
    fin_proceso,  //fin del proceso
	obtener_valor,//obtener_valor [identificador de variable compartida]
	grabar_valor,//grabar_valor [identificador de variable compartida] [valor a grabar]
	cpuwait,//wait [identificador de semáforo]
	cpusignal,//signal [identificador de semáforo]
	entrada_salida //entrada_salida [identificador de dispositivo] [unidades de tiempo a utilizar]
} t_msjcpu;

typedef struct {
	int cpu_socket_descriptor;
	t_PCB *pcb;
	t_msjcpu msj;

} t_cpu;

/*
 * creo struct para manejar entrada salida
 * una lista por dispositivo que adentro tiene una cola por cada uno.
 */
typedef struct {
	char* dispositivo;
	int sleep;
	t_queue *cola;
} t_entradasalida;



void *recNew();
void *recReady();
void *recBlock();

void *recEjecucion(); //es el exec
void *recExit();

void deReadyaExec();
void Planificacion();

int obtener_cantidad_paginas_programa(t_metadata_program*, int bytes);
void iniciar_algoritmo_planificacion();

#endif /* NUCLEO_H_ */

