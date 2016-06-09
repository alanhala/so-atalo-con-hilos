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
       uint32_t respuesta_correcta;
}__attribute__((packed)) t_respuesta_iniciar_programa_en_UMC;

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
t_stream *serializar_enviar_PCB_a_CPU(t_pcb *unPCB);



//Funciones Auxiliares
uint32_t obtiene_cantidad_elementos_stack(t_list *stack_index);
uint32_t obtiene_sizeof_instrucciones(t_intructions *instrucciones);
void obtiene_elementos_del_stack(t_list *stack_index, int elementos_del_stack[]);
t_puntero_instruccion obtiene_primera_instruccion(t_intructions *instruccion);
t_size obtiene_offset (t_intructions *instruccion);

#endif /* PROTOCOLOKERNEL_H_ */
