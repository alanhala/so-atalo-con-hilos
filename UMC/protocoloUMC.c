/*
 * protocoloUMC.c
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */


void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case (2):
			estructuraDestino = deserializar_pedido_bytes_de_una_pagina_a_UMC (datos);
			break;
	}

	return estructuraDestino;
}


t_solicitar_bytes_de_una_pagina_a_UMC *deserializar_pedido_bytes_de_una_pagina_a_UMC(char *datos){

	int		tmpsize = 0,
			offset = 0;

	const int desplazamientoHeader = 5;		//Offset inicial para no deserealizar tipo (1 byte) y length (4 bytes)

	t_solicitar_bytes_de_una_pagina_a_UMC *pedido = malloc(sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));
	memset(pedido,0, sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));

	memcpy(&pedido->pagina, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	memcpy(&pedido->offset, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(&pedido->size, datos+offset, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;

	return pedido;
}
