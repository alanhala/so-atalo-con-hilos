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

//Declaracion estructuras -- Inicio
typedef struct {
	uint32_t pagina;	//Numero de pagina
	uint32_t offset;	//Offset de la pagina
	uint32_t size; 		//Tamano de los datos a escribir
}__attribute__((packed)) solicitar_bytes_de_una_pagina;



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


void valido_conexion_con_umc();

int correrTestSerializacion(){

CU_initialize_registry();

	CU_pSuite serializacion = CU_add_suite("Suite Serializacion", NULL, NULL);
	CU_add_test(serializacion,"uno", valido_conexion_con_umc);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

void valido_conexion_con_umc(){

	int umc_socket_descriptor = create_client_socket_descriptor("localhost","2001");


	t_PCB *pcb = malloc(sizeof(t_PCB));
	memset(pcb,0,sizeof(t_PCB));

	pcb->pid = 463;
	pcb->estado = "Atalo con Hilos";
	pcb->iPointer = 99;


	t_stream *buffer = serializar_mensaje(1,pcb);
	int bytes= send(umc_socket_descriptor, buffer->datos, 30, 0);


	CU_ASSERT_TRUE(bytes > 0);

}

t_stream *serializar_mensaje(int tipo,void* unaEstructura) {

	t_stream *stream;

	switch(tipo){
	case (1):
			stream = serializarPCB((t_PCB *)unaEstructura);
			break;
	}

	return stream;
}



/*
 * Nombre: serializarPCB/1
 * Funcion: recibe un PCB y arma un t_stream con esa info
 * Argumentos:
 * 		- PCB
 *
 * Devuelve:
 * 		- Stream (buffer con el PCB serializado).
 */

t_stream *serializarPCB(t_PCB *unPCB) {

	size_t sizeEstado = strlen(unPCB->estado)+1; //Longitud de la cadena mas el caracter de terminacion nulo

	uint32_t 	tmpsize = 0,
				offset = 0;

	uint32_t 	pcbSize = sizeof(uint32_t) +	//Tamano del PId
				sizeof(uint32_t) +   		//Tamano del IPointer
				sizeEstado;					//Tamano del char* para Estado

	int streamSize 	= pcbSize + 			//Tamano del PCB
					sizeof(uint32_t) + 		//Tamano del largo del stream
					sizeof(uint8_t) + 1;   	//Tamano del tipo

	t_stream *stream = malloc(streamSize);

	memset(stream, 0,streamSize+1);
	stream->size = streamSize;
	//stream->size = pcbSize;
	stream->datos = malloc(streamSize);
	memset(stream->datos,0,streamSize);

	char* estado = malloc(streamSize);

	uint8_t tipo = 1;
	uint32_t processID = unPCB->pid;
	uint32_t instructionPointer = unPCB->iPointer;
	char *estadoProceso = unPCB->estado;

	memcpy(estado,&tipo,tmpsize=sizeof(uint8_t));
	offset+=tmpsize;

	memcpy(estado+offset,&pcbSize,tmpsize=sizeof(uint32_t));
	offset+=tmpsize;

	memcpy(estado+offset, &processID, tmpsize=sizeof(uint32_t));
	offset += tmpsize;

	memcpy(estado+offset, estadoProceso, tmpsize=sizeEstado);
	offset += tmpsize;

	char endString='\0';
	memcpy(estado+offset,&endString,1);

	memcpy(estado+offset, &instructionPointer, tmpsize=sizeof(uint32_t));
	offset += tmpsize;

	stream->datos = estado;

	return stream;
}
