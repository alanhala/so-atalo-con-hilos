/*
 * protocoloCPU.h
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#ifndef PROTOCOLOCPU_H_
#define PROTOCOLOCPU_H_

#include <netdb.h>
#include <parser/metadata_program.h>
#include <parser/parser.h>
#include "cpu.h"

typedef struct {
	uint32_t pagina;	//Numero de pagina
	uint32_t offset;	//Offset de la pagina
	uint32_t size; 		//Tamano de los datos a leer
}__attribute__((packed)) t_solicitar_bytes_de_una_pagina_a_UMC;

typedef struct {
	uint32_t un_numero;
}__attribute__((packed)) t_respuesta_cambio_de_proceso;

typedef struct {
	uint32_t pagina;	//Numero de pagina
	uint32_t offset;	//Offset de la pagina
	uint32_t size; 		//Tamano de los datos a escribir
	char * buffer;		//buffer que quiero escribir de la pagina
}__attribute__((packed)) t_escribir_bytes_de_una_pagina_en_UMC;

typedef struct {
	int escritura_correcta; //0 si ok, -1 si no se pudo escribir la memoria (page fault?)
}__attribute__((packed)) t_respuesta_escribir_bytes_de_una_pagina_en_UMC;

typedef struct {
	uint8_t tipo;
	uint32_t length;
}__attribute__((packed)) t_header;

typedef struct {
	uint32_t pid;
}__attribute__((packed)) t_cambio_de_proceso;

typedef struct {
	uint32_t pid;
	uint32_t program_counter;
	t_list* stack_index;
	t_direccion_virtual_memoria *stack_last_address;
	uint32_t stack_size;
	uint32_t used_pages;
	uint32_t instructions_size;
	t_intructions* instructions_index;
	t_list* label_index;
	uint32_t program_finished;
	uint32_t quantum;
	uint32_t quantum_sleep;
}__attribute__((packed)) t_PCB_serializacion;

typedef struct {
	uint32_t size;
	char *datos;
} t_stream;

typedef struct {
	char *bytes_de_una_pagina;
}__attribute__((packed)) t_respuesta_bytes_de_una_pagina_a_CPU;

typedef struct {
	char *texto_a_imprimir;
}__attribute__((packed)) t_imprimir_texto_en_cpu;




void *deserealizar_mensaje(uint8_t tipo, char* datos);
t_header *deserializar_header(char *header);
t_respuesta_bytes_de_una_pagina_a_CPU *deserializar_respuesta_bytes_de_una_pagina_a_CPU(char *datos);
t_respuesta_escribir_bytes_de_una_pagina_en_UMC *deserializar_respuesta_escribir_bytes_de_una_pagina_en_UMC(char *datos);
t_respuesta_cambio_de_proceso *deserealizar_respuesta_cambio_de_proceso(char *datos);
t_PCB_serializacion *deserializar_PCB(char *datos);
uint32_t deserealizar_int(char* datos);
t_stream *serializar_escribir_bytes_de_una_pagina_en_UMC(t_escribir_bytes_de_una_pagina_en_UMC * unaEstructura);
t_stream *serializar_cambio_de_proceso (t_cambio_de_proceso* unCambioDeProceso);
t_stream *serializar_pedido_bytes_de_una_pagina_a_UMC(t_solicitar_bytes_de_una_pagina_a_UMC *pedido);
t_stream *serializar_mensaje(int tipo,void* unaEstructura);
t_stream *serializar_imprimir_texto_a_cpu(t_imprimir_texto_en_cpu *unaEstructura);
t_stream *serializar_PCB(t_PCB_serializacion *unPCB);

//Funciones Auxiliares
t_list *arma_stack_del_PCB(int elemento_del_stack);
t_intructions carga_instructions_index(t_puntero_instruccion primera_instruccion,t_size offset_instrucciones);
t_size obtiene_offset (t_intructions instruccion);
t_puntero_instruccion obtiene_primera_instruccion(t_intructions instruccion);
uint32_t obtiene_sizeof_instrucciones(t_intructions *instrucciones);

#endif /* PROTOCOLOCPU_H_ */

