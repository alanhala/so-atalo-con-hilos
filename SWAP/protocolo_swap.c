/*
 * protocolo_swap.c
 *
 *  Created on: 25/5/2016
 *      Author: utnso
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "socket.h"
#include "protocolo_swap.h"



void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case (20):
			estructuraDestino =  deserializar_iniciar_programa_en_swap(datos);
			break;

	case (22):
			estructuraDestino =  deserializar_lectura_pagina_swap(datos);
			break;

	case (26):
			estructuraDestino =  deserializar_escribir_pagina_swap(datos);
			break;

	}

	return estructuraDestino;
}

t_escribir_pagina_swap * deserializar_escribir_pagina_swap(char *datos){
	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_escribir_pagina_swap *escritura = malloc(sizeof(t_escribir_pagina_swap));
	memset(escritura,0, sizeof(t_escribir_pagina_swap));


	memcpy(&escritura->pid, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&escritura->pagina, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;


	for(tamanoDato = 0; (datos+offset)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	escritura->datos = malloc(tamanoDato+1);
	memset(escritura->datos,0,tamanoDato+1);

	memcpy(escritura->datos, datos+offset, tmpsize=tamanoDato+1);
	offset+=tamanoDato+1;
	offset+=desplazamientoHeader;

	char endString = '\0';
	memcpy(escritura->datos+offset,&endString,1);


	return escritura;

}


t_leer_pagina_swap * deserializar_lectura_pagina_swap(char * datos){
	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_leer_pagina_swap *pedido = malloc(sizeof(t_leer_pagina_swap));
	memset(pedido,0, sizeof(t_leer_pagina_swap));


	memcpy(&pedido->pid, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&pedido->pagina, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	return pedido;
}



t_iniciar_programa_en_swap * deserializar_iniciar_programa_en_swap(char *datos){

	int		tmpsize = 0,
			offset = 0,
			tamanoDato = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_iniciar_programa_en_swap *pedido = malloc(sizeof(t_iniciar_programa_en_swap));
	memset(pedido,0, sizeof(t_iniciar_programa_en_swap));


	memcpy(&pedido->pid, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&pedido->paginas_necesarias, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;


	for(tamanoDato = 0; (datos+offset)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre

	pedido->codigo_programa = malloc(tamanoDato+1);
	memset(pedido->codigo_programa,0,tamanoDato+1);

	memcpy(pedido->codigo_programa, datos+offset, tmpsize=tamanoDato+1);
	offset+=tamanoDato+1;
	offset+=desplazamientoHeader;

	char endString = '\0';
	memcpy(pedido->codigo_programa+offset,&endString,1);


	return pedido;


}
