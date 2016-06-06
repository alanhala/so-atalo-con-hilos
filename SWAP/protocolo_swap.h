/*
 * protocolo_swap.h
 *
 *  Created on: 25/5/2016
 *      Author: utnso
 */

#ifndef PROTOCOLO_SWAP_H_
#define PROTOCOLO_SWAP_H_

typedef struct {
	uint32_t size;
	char *datos;
} t_stream;

typedef struct {
	uint8_t tipo;		//Tipo del Mensaje
	uint32_t length;	//Largo de la totalidad del mensaje (mensaje en si mas largo y tipo)
}__attribute((packed)) t_header;

typedef struct {
  uint32_t pid;
  uint32_t paginas_necesarias;
  char * codigo_programa;
}__attribute__((packed)) t_iniciar_programa_en_swap;

typedef struct {
	uint32_t process_id;
}__attribute__((packed)) t_finalizar_programa_en_swap;

typedef struct {
	uint32_t resultado;
}__attribute__((packed)) t_respuesta_finalizar_programa_swap;

typedef struct {
  int cargado_correctamente; // 0 si ok, -1 si no ok
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
t_stream * serializar_respuesta_leer_pagina_swap(t_respuesta_leer_pagina_swap *respuesta);
t_leer_pagina_swap *deserializar_lectura_pagina_swap(char *datos);
t_finalizar_programa_en_swap *deserializar_finalizar_programa_swap(char *datos);
t_stream *serializar_respuesta_finalizar_programa_swap(t_respuesta_finalizar_programa_swap *respuesta);
t_stream *serializar_mensaje(int tipo, void* unaEstructura);
t_stream * serializar_respuesta_escribir_pagina_swap(t_respuesta_escribir_pagina_swap *respuesta);
t_escribir_pagina_swap * deserializar_escribir_pagina_swap(char *datos);
t_iniciar_programa_en_swap * deserializar_iniciar_programa_en_swap(char *datos);
t_stream *serializar_respuesta_iniciar_programa_en_swap(t_respuesta_iniciar_programa_en_swap *unaEstructura);









#endif /* PROTOCOLO_SWAP_H_ */
