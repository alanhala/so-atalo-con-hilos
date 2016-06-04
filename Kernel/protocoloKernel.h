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
	uint8_t tipo;
	uint32_t length;
}__attribute__((packed)) t_header;

typedef struct {
	uint32_t size;
	char *datos;
} t_stream;


void *deserealizar_mensaje(uint8_t tipo, char* datos);








#endif /* PROTOCOLOKERNEL_H_ */
