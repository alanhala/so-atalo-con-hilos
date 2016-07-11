/*
 * protocoloKernel.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef PROTOCOLOKERNEL_H_
#define PROTOCOLOKERNEL_H_

#include "kernel.h"


typedef struct {
       uint32_t motivo;
}__attribute__((packed)) t_finalizar_programa_en_consola;

typedef struct {
	uint32_t process_id;
	uint32_t cantidad_de_paginas;
	char *codigo_de_programa;
}__attribute__((packed)) t_inicio_de_programa_en_UMC;

typedef struct {
	uint32_t process_id;
}__attribute__((packed)) t_finalizar_programa_en_UMC;

typedef struct {
	uint32_t respuesta_correcta;
}__attribute__((packed)) t_respuesta_finalizar_programa_en_UMC;

typedef struct {
       char *codigo_de_programa;
}__attribute__((packed)) t_iniciar_programa_en_kernel;

typedef struct {
       uint32_t respuesta_correcta;
}__attribute__((packed)) t_respuesta_iniciar_programa_en_kernel;

typedef struct {
       int32_t respuesta_correcta;
}__attribute__((packed)) t_respuesta_iniciar_programa_en_UMC;

typedef struct {
	uint32_t pid;
	uint32_t program_counter;
	t_list* stack_index;
	t_virtual_address *stack_last_address;
	uint32_t stack_size;
	uint32_t used_pages;
	uint32_t instructions_size;
	t_intructions* instructions_index;
	t_list* label_index;
	uint32_t program_finished;
	uint32_t quantum;
	uint32_t quantum_sleep;
	uint32_t mensaje;
	char *valor_mensaje;
	uint32_t cantidad_operaciones;
	int32_t resultado_mensaje;
	uint32_t valor_de_la_variable_compartida;
	uint32_t cpu_unplugged;
}__attribute__((packed)) t_PCB_serializacion;

typedef struct {
	uint8_t tipo;
	uint32_t length;
}__attribute__((packed)) t_header;

typedef struct {
	uint32_t size;
	char *datos;
} t_stream;

void *deserealizar_mensaje(uint8_t tipo, char* datos);
t_header *deserializar_header(char *header);
t_iniciar_programa_en_kernel *deserealizar_iniciar_programa_en_kernel (char *datos);
t_respuesta_iniciar_programa_en_UMC *deserealizar_respuesta_iniciar_programa_en_UMC(char *datos);
t_respuesta_finalizar_programa_en_UMC *deserealizar_respuesta_finalizar_programa_en_UMC(char *datos);
t_stream *serializar_finalizar_programa_en_UMC(t_finalizar_programa_en_UMC *finalizar_programa);
t_stream *serializar_respuesta_inicio_de_programa_en_kernel(t_respuesta_iniciar_programa_en_kernel *respuesta);
t_stream *serializar_inicio_de_programa_en_UMC(t_inicio_de_programa_en_UMC *inicio_de_programa);
t_stream *serializar_mensaje(int tipo, void *unaEstructura);
t_stream *serializar_PCB(t_PCB_serializacion *unPCB);
t_stream *serializar_finalizar_consola(t_finalizar_programa_en_consola *unaEstructura);
t_PCB_serializacion *deserializar_PCB(char *datos);



//Funciones Auxiliares
uint32_t obtiene_cantidad_elementos_stack(t_list *stack_index);
uint32_t obtiene_sizeof_instrucciones(t_intructions *instrucciones);
void obtiene_elementos_del_stack(t_list *stack_index, int elementos_del_stack[]);
t_puntero_instruccion obtiene_primera_instruccion(t_intructions instruccion);
t_size obtiene_offset (t_intructions instruccion);
void serializa_lista_de_elementos_de_la_pila(t_list *stack_element);
t_intructions carga_instructions_index(t_puntero_instruccion primera_instruccion,t_size offset_instrucciones);

#endif /* PROTOCOLOKERNEL_H_ */
