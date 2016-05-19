/*
 * protocoloCPU.h
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#ifndef PROTOCOLOCPU_H_
#define PROTOCOLOCPU_H_

typedef struct {
	uint32_t pagina;	//Numero de pagina
	uint32_t offset;	//Offset de la pagina
	uint32_t size; 		//Tamano de los datos a escribir
}__attribute__((packed)) t_solicitar_bytes_de_una_pagina_a_UMC;

typedef struct {
	uint32_t size;
	char *datos;
} t_stream;

#endif /* PROTOCOLOCPU_H_ */
