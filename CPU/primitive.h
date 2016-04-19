/*
 * primitive.h
 *
 *  Created on: 18/4/2016
 *      Author: utnso
 */

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

#include <parser/parser.h>
#include <stdio.h>

t_puntero primitive_definirVariable(t_nombre_variable variable);

t_puntero primitive_obtenerPosicionVariable(t_nombre_variable variable);

t_valor_variable primitive_dereferenciar(t_puntero puntero);


void primitive_asignar(t_puntero puntero, t_valor_variable variable);

void primitive_imprimir(t_valor_variable valor);

void primitive_imprimirTexto(char* texto);

#endif /* PRIMITIVE_H_ */
