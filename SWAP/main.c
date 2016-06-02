/*
 * main.c
 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

/*
 * main.c


 *
 *  Created on: 20/4/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "socket.h"
#include "swap.h"
#include "protocolo_swap.h"

#define BACKLOG 10

void UMC_connection(t_swap* swap);

int main(int argc, char *argv[]) {
//	run_specs();
	t_swap* swap = create_swap("config_file.txt");
	UMC_connection(swap);
	return 0;
}

void UMC_connection(t_swap* swap) {
	//Solo se va a conectar un proceso UMC

	int server_socket_descriptor = create_server_socket_descriptor("192.168.0.33",
			swap->port,	10);

	int umc_socket_descriptor = accept_connection(server_socket_descriptor);

	while (1) {

		t_header *aHeader = malloc(sizeof(t_header));

		char 	buffer_header[5];	//Buffer donde se almacena el header recibido

		int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
				bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

		bytes_recibidos_header = recv(umc_socket_descriptor, buffer_header, 5, MSG_PEEK);

		char buffer_recv[buffer_header[1]]; 	//El buffer para recibir el mensaje se crea con la longitud recibida

		if (buffer_header[0] == 1) {

			int bytes_recibidos = recv(umc_socket_descriptor, buffer_recv, buffer_header[1], 0);

			t_iniciar_programa_en_swap *carga = malloc(sizeof(t_iniciar_programa_en_swap));

			carga = (t_iniciar_programa_en_swap *)deserealizar_mensaje(buffer_header[0], buffer_recv);

			int resultado_carga =initialize_program(swap, carga->pid, carga->paginas_necesarias, carga->codigo_programa);

			t_respuesta_iniciar_programa_en_swap *respuesta_inicio_programa = malloc(sizeof(t_respuesta_iniciar_programa_en_swap));

			memset(respuesta_inicio_programa,0,sizeof(t_respuesta_iniciar_programa_en_swap));

			respuesta_inicio_programa->cargado_correctamente = resultado_carga;

			t_stream *buffer = serializar_mensaje(2,respuesta_inicio_programa);

			int bytes= send(umc_socket_descriptor, buffer->datos, buffer->size, 0);

		}

		if (buffer_header[0] == 3) {

			int bytes_recibidos = recv(umc_socket_descriptor, buffer_recv, buffer_header[1], 0);

			t_leer_pagina_swap *lectura = malloc(sizeof(t_leer_pagina_swap));

			lectura = (t_leer_pagina_swap*)deserealizar_mensaje(buffer_header[0], buffer_recv);

			char *contenido = read_page(swap, lectura->pid, lectura->pagina);

			t_respuesta_leer_pagina_swap *respuesta_bytes = malloc(sizeof(t_respuesta_leer_pagina_swap));

			memset(respuesta_bytes,0,sizeof(t_respuesta_leer_pagina_swap));

			respuesta_bytes->datos = contenido;

			t_stream *buffer = serializar_mensaje(4,respuesta_bytes);

			int bytes_sent = send(umc_socket_descriptor, buffer->datos, buffer->size, 0);

		}

		if (buffer_header[0] == 5) {

			int bytes_recibidos = recv(umc_socket_descriptor, buffer_recv, buffer_header[1], 0);

			t_escribir_pagina_swap * carga = malloc(sizeof(t_escribir_pagina_swap));

			carga = (t_escribir_pagina_swap*)deserealizar_mensaje(buffer_header[0], buffer_recv);

			int res = write_page(swap, carga->pid, carga->pagina, carga->datos);

			t_respuesta_escribir_pagina_swap *respuesta = malloc(sizeof(t_respuesta_escribir_pagina_swap));

			memset(respuesta,0,sizeof(t_respuesta_escribir_pagina_swap));

			respuesta->escritura_correcta = res;

			t_stream *buffer_escritura = serializar_mensaje(6,respuesta);

			int bytes = send(umc_socket_descriptor, buffer_escritura->datos, buffer_escritura->size, 0);

		}

		if (buffer_header[0] == -1) {
			break;
		}

	}
}

