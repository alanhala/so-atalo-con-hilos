/*
 * protocoloKernel.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef PROTOCOLOKERNEL_H_
#define PROTOCOLOKERNEL_H_

typedef struct {
	uint32_t process_id;
	uint32_t cantidad_de_paginas;
	char *codigo_de_programa;
}__attribute__((packed)) t_inicio_de_programa_en_UMC;

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

t_stream *serializar_mensaje(int tipo, void *unaEstructura);

void *deserealizar_mensaje(uint8_t tipo, char* datos);
t_iniciar_programa_en_kernel *deserealizar_iniciar_programa_en_kernel (char *datos);
t_stream *serializar_respuesta_inicio_de_programa_en_kernel(t_respuesta_iniciar_programa_en_kernel *respuesta);
t_stream *serializar_inicio_de_programa_en_UMC(t_inicio_de_programa_en_UMC *inicio_de_programa);
t_respuesta_iniciar_programa_en_UMC *deserealizar_respuesta_iniciar_programa_en_UMC(char *datos);







#endif /* PROTOCOLOKERNEL_H_ */
