
#include <commons/log.h>
#include <string.h>
#include <stdint.h>

#include "primitive.h"

static const char* DEFINICION_VARIABLES = "variables a, b, c";
static const char* ASIGNACION = "a = b + 12";
static const char* IMPRIMIR = "print b";
static const char* IMPRIMIR_TEXTO = "textPrint foo\n";

AnSISOP_funciones functions = {
	.AnSISOP_definirVariable	= primitive_definirVariable,
	.AnSISOP_obtenerPosicionVariable= primitive_obtenerPosicionVariable,
	.AnSISOP_dereferenciar	= primitive_dereferenciar,
	.AnSISOP_asignar	= primitive_asignar,
	.AnSISOP_imprimir	= primitive_imprimir,
	.AnSISOP_imprimirTexto	= primitive_imprimirTexto,
};

AnSISOP_kernel kernel_functions = { };

void correrDefinirVariables() {
	printf("Ejecutando '%s'\n", DEFINICION_VARIABLES);
	analizadorLinea(strdup(DEFINICION_VARIABLES), &functions, &kernel_functions);
	printf("================\n");
}

void correrAsignar() {
	printf("Ejecutando '%s'\n", ASIGNACION);
	analizadorLinea(strdup(ASIGNACION), &functions, &kernel_functions);
	printf("================\n");
}

void correrImprimir() {
	printf("Ejecutando '%s'\n", IMPRIMIR);
	analizadorLinea(strdup(IMPRIMIR), &functions, &kernel_functions);
	printf("================\n");
}

void correrImprimirTexto() {
	printf("Ejecutando '%s'", IMPRIMIR_TEXTO);
	analizadorLinea(strdup(IMPRIMIR_TEXTO), &functions, &kernel_functions);
	printf("================\n");
}
/*
int main(int argc, char **argv) {
	correrAsignar();
	correrDefinirVariables();
	correrImprimir();
	correrImprimirTexto();

	return 0;
}

*/
