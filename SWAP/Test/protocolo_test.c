

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


void serializacion_carga_programa_swap_test();
void serializacion_leer_pagina_swap_test();
void serializacion_escribir_pagina_swap_test();

int correr_protocolo_test(){

	CU_initialize_registry();


	CU_pSuite protocolo = CU_add_suite("Suite protocolo", NULL, NULL);
	//CU_add_test(protocolo,"uno", serializacion_carga_programa_swap_test);
	//CU_add_test(protocolo,"dos", serializacion_leer_pagina_swap_test);
	CU_add_test(protocolo,"tres", serializacion_escribir_pagina_swap_test);


	CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}



void serializacion_escribir_pagina_swap_test(){
	int server_socket_descriptor = create_server_socket_descriptor("2033", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[50];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 50, 0);


	t_escribir_pagina_swap * carga = malloc(sizeof(t_escribir_pagina_swap));
	carga = (t_escribir_pagina_swap*)deserealizar_mensaje(26, recv_buffer);

	CU_ASSERT_TRUE(carga->pid == 0);
	CU_ASSERT_TRUE(carga->pagina == 10);
	CU_ASSERT_TRUE(!strcmp(carga->datos,"datos"));

}


void serializacion_carga_programa_swap_test(){
	int server_socket_descriptor = create_server_socket_descriptor("2031", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[50];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 50, 0);


	t_iniciar_programa_en_swap * carga = malloc(sizeof(t_iniciar_programa_en_swap));
	carga = (t_iniciar_programa_en_swap*)deserealizar_mensaje(20, recv_buffer);

	CU_ASSERT_TRUE(carga->pid == 0);
	CU_ASSERT_TRUE(carga->paginas_necesarias == 10);
	CU_ASSERT_TRUE(!strcmp(carga->codigo_programa,"codigo del programa"));

}


void serializacion_leer_pagina_swap_test(){
	int server_socket_descriptor = create_server_socket_descriptor("2032", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);

	char recv_buffer[50];
	int bytesrecv =recv(client_socket_descriptor, recv_buffer, 50, 0);


	t_leer_pagina_swap * lectura = malloc(sizeof(t_leer_pagina_swap));
	lectura = (t_leer_pagina_swap*)deserealizar_mensaje(22, recv_buffer);

	CU_ASSERT_TRUE(lectura->pid == 0);
	CU_ASSERT_TRUE(lectura->pagina == 10);


}
