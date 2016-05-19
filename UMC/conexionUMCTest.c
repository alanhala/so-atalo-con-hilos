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

//Declaracion estructuras -- Inicio
typedef struct {
	uint8_t tipo;
	uint32_t length;
	void *payload;
}__attribute__((packed)) t_mensaje;

typedef struct {
	uint8_t tipo;
	uint32_t length;
} __attribute__((__packed__)) t_header;

typedef struct {
	uint32_t size;
	char *datos;
} t_stream;

typedef struct {
	uint32_t pid;
	char *estado;
	uint32_t iPointer;
} __attribute__((__packed__)) t_PCB;

//Declaracion estructuras -- Fin

//Declaracion Funciones -- Inicio
t_mensaje *crearMensaje(uint8_t tipo);
void destruirMensaje(t_mensaje *mensaje);
void ponerDatosEnMensaje(t_mensaje *mensaje, void *datos, uint32_t length);
void *obtenerDatosDeMensaje(t_mensaje *mensaje);
t_stream *serializar_mensaje(int tipo, void *unaEstructura);
t_stream *serializarPCB(t_PCB *unPCB);
void *deserealizar_mensaje(uint8_t tipo, char* datos);
t_header deserealizarHeader(char * header);
t_header crearHeader(uint8_t tipo, uint32_t tamanoDatos);
t_PCB *deserealizarPCB(char *datos);

//Declaracion Funciones -- Fin



void valido_que_se_conecta_cpu();

int correrTestSerializacion(){

	CU_initialize_registry();


	CU_pSuite serializacion = CU_add_suite("Suite Serializacion", NULL, NULL);
	CU_add_test(serializacion,"uno", valido_que_se_conecta_cpu);


	CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}
void valido_que_se_conecta_cpu(){

	int server_socket_descriptor = create_server_socket_descriptor("2001", 10);

	int client_socket_descriptor = accept_connection(server_socket_descriptor);


	char recvBuffer[30];
	memset(recvBuffer,0,sizeof(recvBuffer));
	int bytesrecv =recv(client_socket_descriptor, recvBuffer, 30, 0);


	t_PCB *unPCB = malloc(sizeof(unPCB));

	unPCB = (t_PCB *)deserealizar_mensaje(1,recvBuffer);


	CU_ASSERT_TRUE(bytesrecv > 0);
	CU_ASSERT_TRUE(unPCB->pid == 463);
	CU_ASSERT_TRUE(unPCB->iPointer == 99);
	CU_ASSERT_TRUE(!strcmp(unPCB->estado,"Atalo con Hilos"));
}


/*
 * Nombre: deserealizarMensaje/2
 * Funcion: segun el tipo de estructura, entra al proceso de deserializacion correspondiente
 * Argumentos:
 * 		- tipo
 * 		- char * datos: array plano que se toma del socket del servidor
 *
 * Devuelve:
 * 		estructuraDestino (estructura donde se guarda el paquete).
 * 		--> Recordar hacer free de la estructuraDestino luego de utilizar esta funcion
 */

void *deserealizar_mensaje(uint8_t tipo, char* datos) {

	void* estructuraDestino;

	switch(tipo){
	case (1):
			estructuraDestino = deserealizarPCB (datos);
			break;
	}

	return estructuraDestino;
}



/*
 * Nombre: deserealizarPCB/1
 * Funcion: recibe el array plano del socket y arma el PCB
 * Argumentos:
 * 		- char *datos
 *
 * Devuelve:
 * 		una estructura de tipo t_PCB
 */

t_PCB *deserealizarPCB(char *datos){

	int tamanoDato = 0,
		tmpsize = 0,
		offset = 0;

	const int desplazamientoHeader = 5;

	t_PCB *pcb = malloc(sizeof(t_PCB));
	memset(pcb,0, sizeof(t_PCB));

	memcpy(&pcb->pid, datos+desplazamientoHeader, tmpsize = sizeof(uint32_t));
	offset+=tmpsize;
	offset+=desplazamientoHeader;

	for(tamanoDato = 0; (datos+offset)[tamanoDato] != '\0';tamanoDato++);//incremento tamanoDato, hasta el tamaÃ±o del nombre
	pcb->estado = malloc(tamanoDato+1);

	memset(pcb->estado,0,tamanoDato+1);
	memcpy(pcb->estado, datos+offset, tamanoDato+1);
	offset+=tamanoDato+1;

	memcpy(&pcb->iPointer,datos+offset,sizeof(uint32_t));

	return pcb;
}

