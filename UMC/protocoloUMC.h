/*
 * protocoloUMC.h
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */

#ifndef PROTOCOLOUMC_H_
#define PROTOCOLOUMC_H_

typedef struct {
       uint32_t respuesta_correcta;
}__attribute__((packed)) t_respuesta_inicio_de_programa_en_UMC;

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
	uint32_t process_id;
}__attribute__((packed)) t_finalizar_programa_en_swap;

typedef struct {
	uint32_t pagina;	//Numero de pagina
	uint32_t offset;	//Offset de la pagina
	uint32_t size; 		//Tamano de los datos a escribir
}__attribute__((packed)) t_solicitar_bytes_de_una_pagina_a_UMC;

typedef struct {
	char *bytes_de_una_pagina;
}__attribute__((packed)) t_respuesta_bytes_de_una_pagina_a_CPU;

typedef struct {
	uint8_t tipo;
	uint32_t length;
}__attribute__((packed)) t_header;

typedef struct {
	uint32_t pid;
}__attribute__((packed)) t_cambio_de_proceso;

typedef struct {
	uint32_t cambio_correcto;
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
	uint32_t resultado;
}__attribute__((packed)) t_respuesta_finalizar_programa_swap;


typedef struct {
	uint32_t size;
	char *datos;
} t_stream;

typedef struct {
  uint32_t pid;
  uint32_t paginas_necesarias;
  char * codigo_programa;
}__attribute__((packed)) t_iniciar_programa_en_swap;

typedef struct {
  uint32_t cargado_correctamente; // 0 si ok, -1 si no ok
}__attribute__((packed)) t_respuesta_iniciar_programa_en_swap;

typedef struct {
  uint32_t pid;
  uint32_t pagina;
}__attribute__((packed)) t_leer_pagina_swap;

typedef struct {
  char * datos;
}__attribute__((packed)) t_respuesta_leer_pagina_swap;

typedef struct {
  uint32_t pid;
  uint32_t pagina;
  char * datos;
}__attribute__((packed)) t_escribir_pagina_swap;

typedef struct {
  uint32_t escritura_correcta; // 0 si escribi ok, -1 si no ok
}__attribute__((packed)) t_respuesta_escribir_pagina_swap;


void *deserealizar_mensaje(uint8_t tipo, char* datos);
t_header *deserializar_header(char *header);
t_solicitar_bytes_de_una_pagina_a_UMC *deserializar_pedido_bytes_de_una_pagina_a_UMC(char *datos);
t_escribir_bytes_de_una_pagina_en_UMC * deserializar_escribir_bytes_de_una_pagina_en_UMC(char *datos);
t_inicio_de_programa_en_UMC *deserealizar_inicio_de_programa_en_UMC(char *datos);
t_cambio_de_proceso *deserealizar_cambio_de_proceso(char *datos);
t_respuesta_finalizar_programa_swap *deserealizar_respuesta_finalizar_programa_swap(char *datos);
t_finalizar_programa_en_UMC *deserealizar_finalizar_programa_en_UMC(char *datos);
t_respuesta_iniciar_programa_en_swap * deserializar_respuesta_iniciar_programa_en_swap(char* datos);
t_respuesta_leer_pagina_swap *deserializar_respuesta_leer_pagina_swap(char *datos);
t_respuesta_escribir_pagina_swap * deserializar_respuesta_escribir_pagina_swap(char *datos);
t_stream *serializar_respuesta_finalizar_programa_en_UMC(t_respuesta_finalizar_programa_en_UMC *finalizar_programa);
t_stream *serializar_respuesta_bytes_de_una_pagina_a_CPU(t_respuesta_bytes_de_una_pagina_a_CPU *unaEstructura);
t_stream *serializar_respuesta_escribir_bytes_de_una_pagina_en_UMC(t_respuesta_escribir_bytes_de_una_pagina_en_UMC *unaEstructura);
t_stream *serializar_respuesta_cambio_de_proceso(t_respuesta_cambio_de_proceso* unaEstructura);
t_stream *serializar_respuesta_inicio_de_programa_en_UMC(t_respuesta_inicio_de_programa_en_UMC *respuesta);
t_stream *serializar_finalizar_programa_swap(t_finalizar_programa_en_swap *finalizar_programa);
t_stream * serializar_iniciar_programa_en_swap(t_iniciar_programa_en_swap *pedido);
t_stream * serializar_leer_pagina_swap(t_leer_pagina_swap * pedido);
t_stream * serializar_escribir_pagina_swap(t_escribir_pagina_swap * escritura);


#endif /* PROTOCOLOUMC_H_ */
