/*
 * primitive.c
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */
#include <parser/parser.h>
#include <stdio.h>

#include "primitive.h"

static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;

t_puntero primitive_definirVariable(t_nombre_variable variable) {
	printf("definir la variable %c\n", variable);
	return POSICION_MEMORIA;
}

t_puntero primitive_obtenerPosicionVariable(t_nombre_variable variable) {
	printf("Obtener posicion de %c\n", variable);
	return POSICION_MEMORIA;
}

t_valor_variable primitive_dereferenciar(t_puntero puntero) {
	printf("Dereferenciar %d y su valor es: %d\n", puntero, CONTENIDO_VARIABLE);
	return CONTENIDO_VARIABLE;
}
void primitive_asignar(t_puntero puntero, t_valor_variable variable) {
	printf("Asignando en %d el valor %d\n", puntero, variable);
}

void primitive_imprimir(t_valor_variable valor) {
	printf("Imprimir %d\n", valor);
}

void primitive_imprimirTexto(char* texto) {
	printf("ImprimirTexto: %s", texto);
}
