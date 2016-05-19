/*
 * testProtocolo.c
 *
 *  Created on: 18/5/2016
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
#include <commons/collections/list.h>
#include <semaphore.h>
#include "CUnit/Basic.h"
#include "socket.h"
#include "protocoloCPU.h"


void serializacion_lectura_umc();

int correrTestSerializacion(){

CU_initialize_registry();

	CU_pSuite serializacion = CU_add_suite("Suite Serializacion", NULL, NULL);
	CU_add_test(serializacion,"dos", serializacion_lectura_umc);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}


void serializacion_lectura_umc(){
		int umc_socket_descriptor = create_client_socket_descriptor("localhost","2005");


		t_solicitar_bytes_de_una_pagina_a_UMC *pedido = malloc(sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));
		memset(pedido,0,sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));

		pedido->pagina = 73;
		pedido->offset = 23;
		pedido->size = 13;

		t_stream *buffer = serializar_mensaje(2,pedido);
		int bytes= send(umc_socket_descriptor, buffer->datos, 20, 0);

		CU_ASSERT_TRUE(bytes > 0);

		char recv_buffer[50];
		recv(umc_socket_descriptor, recv_buffer, 50, 0);


		t_respuesta_bytes_de_una_pagina_a_CPU * respuesta = malloc(sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));
		respuesta = (t_respuesta_bytes_de_una_pagina_a_CPU*)deserealizar_mensaje(3, recv_buffer);


		CU_ASSERT_TRUE(!strcmp(respuesta->bytes_de_una_pagina, "recibo una respuesta de una pagina"));

}

