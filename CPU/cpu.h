/*
 * cpu.h
 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

#include <parser/metadata_program.h>
#include <parser/parser.h>
#include "primitive.h"
#include <commons/collections/list.h>
#include <netdb.h>
#include "protocoloCPU.h"

int KERNEL_DESCRIPTOR;
int UMC_DESCRIPTOR;

set_umc_socket_descriptor(int socket_descriptor);
set_kernel_socket_descriptor(int socket_descriptor);


typedef struct {
	uint32_t numero_pagina;
	t_intructions instruccion;
} t_indice_instrucciones_elemento;

typedef struct {
	uint32_t pagina;
	uint32_t offset;
} t_direccion_virtual_memoria;

typedef struct {
    uint32_t size;
    t_direccion_virtual_memoria direccion;
} t_dato_en_memoria;

typedef struct {
	t_nombre_variable id;
	t_dato_en_memoria dato;
} t_variable;

typedef struct {
	t_list *argumentos;
	t_list *variables;
	uint32_t posicion_retorno;
	t_direccion_virtual_memoria valor_retorno;
	t_direccion_virtual_memoria direccion_inicial;
} t_stack_element;

typedef struct {
	uint32_t pid;
	uint32_t program_counter;
	uint32_t paginas_codigo;
	uint32_t cantidad_instrucciones;
	t_intructions* indice_instrucciones;
	uint32_t cantidad_etiquetas;
	//t_indice_etiqueta TODO: implementar esto
	t_list *stack;
	t_direccion_virtual_memoria stack_pointer;
	uint32_t stack_size;
	t_direccion_virtual_memoria stack_next_free_space;
} t_PCB;

void execute_next_instruction_for_process();
t_dato_en_memoria get_next_instruction();
char* obtener_instruccion_de_umc(t_dato_en_memoria *instruccion);
void incrementar_next_free_space();
void set_PCB(t_PCB *pcb);
t_PCB* get_PCB();
t_stack_element* create_stack_element();

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable);
t_puntero definirVariable(t_nombre_variable variable);
t_valor_variable dereferenciar(t_puntero puntero);
void asignar(t_puntero direccion_variable, t_valor_variable valor);

char* leer_memoria_de_umc(t_dato_en_memoria *dato);
int escribir_en_umc(t_dato_en_memoria *dato, char* valor);

char* ejecutar_lectura_de_dato_con_iteraciones(void*(*lectura)(t_dato_en_memoria*), t_dato_en_memoria* dato, uint32_t tamanio_pagina);
int ejecutar_escritura_de_dato_con_iteraciones(t_dato_en_memoria *dato, char* valor, uint32_t tamanio_pagina);

#endif /* CPU_H_ */
