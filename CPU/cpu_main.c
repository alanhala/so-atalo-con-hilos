/*
 * cpu_main.c
 *
 *  Created on: 28/4/2016
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
#include "protocoloCPU.h"
#include "cpu.h"

#define KERNELPORT "9000"
#define KERNELIP "localhost"

#define UMCPORT "5000"
#define UMCIP "localhost"


void connect_to_UMC();
void connect_to_Kernel();



int main(int argc, char **argv) {

	set_quantum(3); // me lo tiene qe mandar cpu en cada ejecucion
	set_tamanio_pagina(5);
//	correr_simulacion();
//	while(1){
//			sleep(1000);
//		}
//	return 0;



//	if (argc == 3) {
//	    if (strcmp(argv[2], "-conexiones") == 0){
   			connect_to_UMC();
    		//connect_to_Kernel();
//    	    }
//	}
//
//	if (strcmp(argv[1], "-test") == 0){
		 //correrTest();
		correr_simulacion();
		 return 0;
//		 //correrTestSerializacion();
//		 return 0;
//	}

	connect_to_UMC();
	connect_to_Kernel();

	while (1) {

		t_header *aHeader = malloc(sizeof(t_header));

		char buffer_header[5];	//Buffer donde se almacena el header recibido

		int bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
				bytes_recibidos;//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

		bytes_recibidos_header = recv(KERNEL_DESCRIPTOR, buffer_header, 5,	MSG_PEEK);

		char buffer_recv[buffer_header[1]]; //El buffer para recibir el mensaje se crea con la longitud recibida

		if (buffer_header[0] == 121) {

			int bytes_recibidos = recv(KERNEL_DESCRIPTOR, buffer_recv,
					buffer_header[1], 0);

			t_PCB_serializacion *recibir_pcb = malloc(sizeof(t_PCB_serializacion));

			recibir_pcb = (t_PCB_serializacion * ) deserealizar_mensaje(buffer_header[0], buffer_recv);

			t_PCB *pcb = malloc(sizeof(t_PCB));
			pcb->instructions_index = recibir_pcb->instructions_index;
			pcb->pid = recibir_pcb->pid;
			pcb->instructions_size = recibir_pcb->instructions_size;
			pcb->program_counter = recibir_pcb->program_counter;
			pcb->stack= recibir_pcb->stack_index;
			pcb->used_pages = recibir_pcb->used_pages;
			pcb->stack_size = recibir_pcb->stack_size;

			//HARCODEADO VER CON ALAN Y MATI
			 t_direccion_virtual_memoria *free_space = malloc(sizeof(t_direccion_virtual_memoria));
			free_space->offset = 3;
			free_space->pagina = 20;
			//pcb->stack_pointer = recibir_pcb->stack_pointer;
			pcb->stack_free_space_pointer = free_space;
			set_PCB(pcb);
			int resultado_ejecucion = ejecutar_pcb();

			//ENVIAR EL PCB A KERNEL
		}
	}

	return 0;
}

void connect_to_UMC() {

	int umc_socket_descriptor = create_client_socket_descriptor(UMCIP, UMCPORT);

	set_umc_socket_descriptor(umc_socket_descriptor);
	int a =1;
	send(umc_socket_descriptor, &a, sizeof(int), 0);


}



void connect_to_Kernel() {

	int kernel_socket_descriptor =create_client_socket_descriptor(KERNELIP, KERNELPORT);

	set_kernel_socket_descriptor(kernel_socket_descriptor);
	int a =1;
	send(kernel_socket_descriptor, &a, sizeof(int), 0);

}


