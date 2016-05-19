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
void serializacion_escritura_umc();


int correrTestSerializacion(){

	CU_initialize_registry();


	CU_pSuite serializacion = CU_add_suite("Suite Serializacion", NULL, NULL);
	//CU_add_test(serializacion,"uno", serializacion_lectura_umc);
	CU_add_test(serializacion,"cinco", serializacion_escritura_umc);


	CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}

void serializacion_escritura_umc(){
	int server_socket_descriptor = create_server_socket_descriptor("2007", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[70];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 70, 0);


	t_escribir_bytes_de_una_pagina_en_UMC * escritura = malloc(sizeof(t_escribir_bytes_de_una_pagina_en_UMC));
	escritura = (t_escribir_bytes_de_una_pagina_en_UMC*)deserealizar_mensaje(10, recv_buffer);

	CU_ASSERT_TRUE(escritura->pagina == 5);
	CU_ASSERT_TRUE(escritura->offset == 10);
	CU_ASSERT_TRUE(escritura->size == 50);
	CU_ASSERT_TRUE(!strcmp(escritura->buffer,"escribir contenido de una pagina"));
}

void serializacion_lectura_umc(){

	int server_socket_descriptor = create_server_socket_descriptor("2005", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[20];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 20, 0);

	t_solicitar_bytes_de_una_pagina_a_UMC * pedido = malloc(sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));
	pedido = (t_solicitar_bytes_de_una_pagina_a_UMC*)deserealizar_mensaje(2, recv_buffer);

	CU_ASSERT_TRUE(pedido->pagina == 73);
	CU_ASSERT_TRUE(pedido->offset == 23);
	CU_ASSERT_TRUE(pedido->size == 13);


	//aca le mando la respuesta, deberia ser otro test pero quiero seguir manteniendo la misma conexion servidor

	char * bytes_de_la_pagina = "recibo una respuesta de una pagina"; // aca deberia ir a buscar el contenido de la pagina a UMC
	t_respuesta_bytes_de_una_pagina_a_CPU *respuesta_bytes = malloc(sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));
	memset(respuesta_bytes,0,sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));
	respuesta_bytes->bytes_de_una_pagina = bytes_de_la_pagina;
	t_stream *buffer = serializar_mensaje(3,respuesta_bytes);

	int bytes= send(client_socket_descriptor, buffer->datos, 50, 0);




}



