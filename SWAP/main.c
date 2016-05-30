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

#define LISTENPORT "8000"
#define BACKLOG 10

void UMC_connection(t_swap* swap);

int main(int argc, char *argv[]) {
	run_specs();
//	UMC_connection(swap);
//	return 0;
}

void UMC_connection(t_swap* swap) {
	//Solo se va a conectar un proceso UMC

	int server_socket_descriptor = create_server_socket_descriptor("192.168.0.102",
			LISTENPORT,	10);

	int umc_socket_descriptor = accept_connection(server_socket_descriptor);


	while (1) {
		char code[1];
		recv(umc_socket_descriptor, code, 1, 0);
		if (code[0] == '1') {

			char recv_buffer[200];
			int bytesrecv =recv(umc_socket_descriptor, recv_buffer, 200, 0);


			t_iniciar_programa_en_swap * carga = malloc(sizeof(t_iniciar_programa_en_swap));
			carga = (t_iniciar_programa_en_swap*)deserealizar_mensaje(20, recv_buffer);

			int resultado_carga =initialize_program(swap, carga->pid, carga->paginas_necesarias, carga->codigo_programa);

			t_respuesta_iniciar_programa_en_swap *respuesta_inicio_programa = malloc(sizeof(t_respuesta_iniciar_programa_en_swap));
			memset(respuesta_inicio_programa,0,sizeof(t_respuesta_iniciar_programa_en_swap));
			respuesta_inicio_programa->cargado_correctamente = resultado_carga;
			t_stream *buffer = serializar_mensaje(20,respuesta_inicio_programa);

			int bytes= send(umc_socket_descriptor, buffer->datos, 50, 0);


		}
		if (code[0] == '2') {
			char recv_buffer_lectura[50];
			int bytesrecv_lectura =recv(umc_socket_descriptor, recv_buffer_lectura, 50, 0);


			t_leer_pagina_swap * lectura = malloc(sizeof(t_leer_pagina_swap));
			lectura = (t_leer_pagina_swap*)deserealizar_mensaje(22, recv_buffer_lectura);

			char *contenido = read_page(swap, lectura->pid, lectura->pagina);
			t_respuesta_leer_pagina_swap *respuesta_bytes = malloc(sizeof(t_respuesta_leer_pagina_swap));
			memset(respuesta_bytes,0,sizeof(t_respuesta_leer_pagina_swap));
			respuesta_bytes->datos = contenido;
			t_stream *buffer = serializar_mensaje(22,respuesta_bytes);

			int bytes2= send(umc_socket_descriptor, buffer->datos, 50, 0);

				}
		if (code[0] == '3') {
			char recv_buffer_escritura[50];
			int bytesrecv =recv(umc_socket_descriptor, recv_buffer_escritura, 50, 0);


			t_escribir_pagina_swap * carga = malloc(sizeof(t_escribir_pagina_swap));
			carga = (t_escribir_pagina_swap*)deserealizar_mensaje(26, recv_buffer_escritura);

			int res = write_page(swap, carga->pid, carga->pagina, carga->datos);

			t_respuesta_escribir_pagina_swap *respuesta = malloc(sizeof(t_respuesta_escribir_pagina_swap));
			memset(respuesta,0,sizeof(t_respuesta_escribir_pagina_swap));
			respuesta->escritura_correcta = res;
			t_stream *buffer_escritura = serializar_mensaje(26,respuesta);

			int bytes= send(umc_socket_descriptor, buffer_escritura->datos, 50, 0);


		}

		if (code[0] == '4') {
			break;
		}

	}
}

