

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "CUnit/Basic.h"
#include "../protocolo_swap.h"
#include "../swap.h"
#include "../socket.h"


void serializacion_carga_programa_swap_test();
void serializacion_leer_pagina_swap_test();
void serializacion_escribir_pagina_swap_test();

int correr_protocolo_test(){

	CU_initialize_registry();


	CU_pSuite protocolo = CU_add_suite("Suite protocolo", NULL, NULL);
	CU_add_test(protocolo,"uno", serializacion_carga_programa_swap_test);
	//CU_add_test(protocolo,"dos", serializacion_leer_pagina_swap_test);
	//CU_add_test(protocolo,"tres", serializacion_escribir_pagina_swap_test);


	CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}



void serializacion_escribir_pagina_swap_test(){
	int server_socket_descriptor = create_server_socket_descriptor("localhost","2033", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[50];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 50, 0);


	t_escribir_pagina_swap * carga = malloc(sizeof(t_escribir_pagina_swap));
	carga = (t_escribir_pagina_swap*)deserealizar_mensaje(26, recv_buffer);

	CU_ASSERT_TRUE(carga->pid == 0);
	CU_ASSERT_TRUE(carga->pagina == 10);
	CU_ASSERT_TRUE(!strcmp(carga->datos,"datos"));



	t_respuesta_escribir_pagina_swap *respuesta = malloc(sizeof(t_respuesta_escribir_pagina_swap));
	memset(respuesta,0,sizeof(t_respuesta_escribir_pagina_swap));
	respuesta->escritura_correcta = 0;
	t_stream *buffer = serializar_mensaje(26,respuesta);

	int bytes= send(client_socket_descriptor, buffer->datos, 50, 0);


}


void serializacion_carga_programa_swap_test(){
	int server_socket_descriptor = create_server_socket_descriptor("localhost","2031", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[50];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 50, 0);


	t_iniciar_programa_en_swap * carga = malloc(sizeof(t_iniciar_programa_en_swap));
	carga = (t_iniciar_programa_en_swap*)deserealizar_mensaje(20, recv_buffer);

	CU_ASSERT_TRUE(carga->pid == 0);
	CU_ASSERT_TRUE(carga->paginas_necesarias == 10);
	CU_ASSERT_TRUE(!strcmp(carga->codigo_programa,"codigo del programa"));


	int resultado_de_la_escritura = 0;

	t_respuesta_iniciar_programa_en_swap *respuesta_inicio_programa = malloc(sizeof(t_respuesta_iniciar_programa_en_swap));
	memset(respuesta_inicio_programa,0,sizeof(t_respuesta_iniciar_programa_en_swap));
	respuesta_inicio_programa->cargado_correctamente = resultado_de_la_escritura;
	t_stream *buffer = serializar_mensaje(20,respuesta_inicio_programa);

	int bytes= send(client_socket_descriptor, buffer->datos, 50, 0);

}


void serializacion_leer_pagina_swap_test(){
	int server_socket_descriptor = create_server_socket_descriptor("localhost","2032", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[50];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 50, 0);


	t_leer_pagina_swap * lectura = malloc(sizeof(t_leer_pagina_swap));
	lectura = (t_leer_pagina_swap*)deserealizar_mensaje(22, recv_buffer);

	CU_ASSERT_TRUE(lectura->pid == 0);
	CU_ASSERT_TRUE(lectura->pagina == 10);

	char * bytes_de_la_pagina = "contenido de la pagina"; // aca deberia ir a buscar el contenido de la pagina a swap
	t_respuesta_leer_pagina_swap *respuesta_bytes = malloc(sizeof(t_respuesta_leer_pagina_swap));
	memset(respuesta_bytes,0,sizeof(t_respuesta_leer_pagina_swap));
	respuesta_bytes->datos = bytes_de_la_pagina;
	t_stream *buffer = serializar_mensaje(22,respuesta_bytes);

	int bytes= send(client_socket_descriptor, buffer->datos, 50, 0);

}
