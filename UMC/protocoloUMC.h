/*
 * protocoloUMC.h
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#ifndef PROTOCOLOUMC_H_
#define PROTOCOLOUMC_H_

typedef struct {
	uint32_t pagina;	//Numero de pagina
	uint32_t offset;	//Offset de la pagina
	uint32_t size; 		//Tamano de los datos a escribir
}__attribute__((packed)) t_solicitar_bytes_de_una_pagina_a_UMC;

typedef struct {
	char *bytes_de_una_pagina;
}__attribute__((packed)) t_respuesta_bytes_de_una_pagina_a_CPU;

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

t_solicitar_bytes_de_una_pagina_a_UMC *deserializar_pedido_bytes_de_una_pagina_a_UMC(char *datos);
void *deserealizar_mensaje(uint8_t tipo, char* datos);
t_stream *serializar_respuesta_bytes_de_una_pagina_a_CPU(t_respuesta_bytes_de_una_pagina_a_CPU *unaEstructura);
t_stream *serializar_respuesta_escribir_bytes_de_una_pagina_en_UMC(t_respuesta_escribir_bytes_de_una_pagina_en_UMC *unaEstructura);


t_escribir_bytes_de_una_pagina_en_UMC * deserializar_escribir_bytes_de_una_pagina_en_UMC(datos);
#endif /* PROTOCOLOUMC_H_ */
