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
	uint32_t size; 		//Tamano de los datos a leer
}__attribute__((packed)) t_solicitar_bytes_de_una_pagina_a_UMC;



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
	uint32_t size;
	char *datos;
} t_stream;

typedef struct {
	char *bytes_de_una_pagina;
}__attribute__((packed)) t_respuesta_bytes_de_una_pagina_a_CPU;



t_stream *serializar_pedido_bytes_de_una_pagina_a_UMC(t_solicitar_bytes_de_una_pagina_a_UMC *pedido);
void *serealizar_mensaje(uint8_t tipo, char* datos);
void *deserealizar_mensaje(uint8_t tipo, char* datos);
t_respuesta_bytes_de_una_pagina_a_CPU *deserializar_respuesta_bytes_de_una_pagina_a_CPU(char *datos);
t_respuesta_escribir_bytes_de_una_pagina_en_UMC * deserializar_respuesta_escribir_bytes_de_una_pagina_en_UMC(char *datos);

t_stream *serializar_escribir_bytes_de_una_pagina_en_UMC(t_escribir_bytes_de_una_pagina_en_UMC * unaEstructura);

#endif /* PROTOCOLOCPU_H_ */

