/*
 * protocoloConsola.h
 *
 *  Created on: 4/6/2016
 *      Author: utnso
 */

#ifndef ANSISOP_PROTOCOLOCONSOLA_H_
#define ANSISOP_PROTOCOLOCONSOLA_H_



typedef struct {
	char *codigo_de_programa;
}__attribute__((packed)) t_iniciar_programa_en_kernel;

typedef struct {
	uint32_t respuesta_correcta;
}__attribute__((packed)) t_respuesta_iniciar_programa_en_kernel;

typedef struct {
	char *texto_a_imprimir;
}__attribute__((packed)) t_imprimir_texto_en_consola;

typedef struct {
       uint32_t motivo;
}__attribute__((packed)) t_finalizar_programa_en_consola;

typedef struct {
	uint8_t tipo;
	uint32_t length;
}__attribute__((packed)) t_header;

typedef struct {
	uint32_t size;
	char *datos;
} t_stream;


void *deserealizar_mensaje(uint8_t tipo, char* datos);
t_imprimir_texto_en_consola *  deserealizar_imprimir_texto_en_consola(char *datos);
t_respuesta_iniciar_programa_en_kernel *deserealizar_respuesta_inicio_de_programa_en_kernel(char *datos);
t_finalizar_programa_en_consola * deserealizar_finalizar_consola(char *datos);
t_stream *serializar_mensaje(int tipo, void* unaEstructura);
t_stream *serializar_iniciar_programa_en_kernel(t_iniciar_programa_en_kernel *inicio_programa);



#endif /* ANSISOP_PROTOCOLOCONSOLA_H_ */
