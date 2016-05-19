/*
 * testSerializacion.c
 *
 *  Created on: 18/5/2016
 *      Author: utnso
 */
/*
 * test.c
 *
 *  Created on: 6/5/2016
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
#include "memoriaPrincipal.h"
#include "CUnit/Basic.h"
#include "test.h"
#include "protocoloUMC.h"

void serializacion_lectura_umc();

void valido_que_se_conecta_cpu();

int correrTestSerializacion(){

	CU_initialize_registry();


	CU_pSuite serializacion = CU_add_suite("Suite Serializacion", NULL, NULL);
	CU_add_test(serializacion,"uno", serializacion_lectura_umc);


	CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}


void serializacion_lectura_umc(){

	int server_socket_descriptor = create_server_socket_descriptor("2002", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[20];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 20, 0);

	t_solicitar_bytes_de_una_pagina_a_UMC * pedido = malloc(sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));
	pedido = (t_solicitar_bytes_de_una_pagina_a_UMC*)deserealizar_mensaje(2, recv_buffer);

	CU_ASSERT_TRUE(pedido->pagina == 73);
	CU_ASSERT_TRUE(pedido->offset == 23);
	CU_ASSERT_TRUE(pedido->size == 13);
}

