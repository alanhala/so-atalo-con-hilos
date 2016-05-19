/*
 * protocoloCPU.c
 *
 *  Created on: 19/5/2016
 *      Author: utnso
 */


t_stream *serializar_mensaje(int tipo,void* unaEstructura) {

	t_stream *stream;

	switch(tipo){
	case (2):
			stream = serializar_pedido_bytes_de_una_pagina_a_UMC((t_solicitar_bytes_de_una_pagina_a_UMC *)unaEstructura);
			break;
	}

	return stream;
}



t_stream *serializar_pedido_bytes_de_una_pagina_a_UMC(t_solicitar_bytes_de_una_pagina_a_UMC *pedido){
	uint32_t 	tmpsize = 0,
				offset = 0;

	uint32_t 	size_pedido = 	sizeof(uint32_t) +	//Tamano del numero de pagina
								sizeof(uint32_t) +	//Tamano del offset
								sizeof(uint32_t);	//Tamano del campo datos a escribir

	uint32_t 	streamSize =	sizeof(uint8_t)	+	//Tamano del tipo
								sizeof(uint32_t)+	//Tamano del largo del stream
								size_pedido;		//Tamano del pedido de pagina

	t_stream *stream = malloc(streamSize);

	memset(stream, 0,streamSize);
	stream->size = size_pedido;
	stream->datos = malloc(streamSize);
	memset(stream->datos,0,streamSize);

	uint8_t tipo = 2; 	//Tipo del Mensaje . Fijado estaticamente segun protocolo
	uint32_t numero_pagina = pedido->pagina;
	uint32_t offset_pagina = pedido->offset;
	uint32_t size_pagina = pedido->size;

	memcpy(stream->datos,&numero_pagina,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&offset_pagina,tmpsize(uint32_t));
	offset+=tmpsize;

	memcpy(stream->datos+offset,&size_pagina,tmpsize(uint32_t));
	offset+=tmpsize;

	return stream;
}

