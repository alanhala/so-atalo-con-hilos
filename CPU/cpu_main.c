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
#include <commons/config.h>

void connect_to_UMC();
void connect_to_Kernel();
t_PCB_serializacion * adaptar_pcb_a_serializar(t_PCB * pcb);
void actualizarPCB(t_PCB *pcb, t_PCB_serializacion *recibir_pcb);
void *captador_de_senal_thread();
void sig_handler();
int sigusr_received;

t_log *trace_log_CPU;

char	*kernel_ip,
		*kernel_puerto,
		*umc_ip,
		*umc_puerto;

int levanta_config_cpu(void);

int main(int argc, char **argv) {

	levanta_config_cpu();

	trace_log_CPU = log_create("./Log_de_CPU.txt",
								"cpu_main.c",
								true,
								LOG_LEVEL_TRACE);

	log_trace(trace_log_CPU,"\n\n\n\n\n");

	//set_quantum(3); // me lo tiene qe mandar cpu en cada ejecucion
//	set_tamanio_pagina(5);
//	correr_simulacion();
//	while(1){
//			sleep(1000);
//		}
//	return 0;

//	if (argc == 3) {
//	    if (strcmp(argv[2], "-conexiones") == 0){
//		connect_to_UMC();
//    		connect_to_Kernel();
//    	    }
//	}
//
//	if (strcmp(argv[1], "-test") == 0){
//		 correrTest();
		//correr_simulacion();
//		 return 0;
		 //correrTest();
		//correr_simulacion();
		 //return 0;
//		 //correrTestSerializacion();
//		 return 0;
//	}

	connect_to_UMC();
	connect_to_Kernel();

	log_trace(trace_log_CPU,"Esperando PCB\n");


	pthread_t captador_de_senal;
	pthread_create(&captador_de_senal,NULL,&captador_de_senal_thread,NULL);

	while (1) {

		t_header *a_header = malloc(sizeof(t_header));

		char buffer_header[5];	//Buffer donde se almacena el header recibido

		int bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
				bytes_recibidos;//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

		bytes_recibidos_header = recv(KERNEL_DESCRIPTOR, buffer_header, 5,	MSG_PEEK);

		a_header = deserializar_header(buffer_header);

		int tipo = a_header->tipo;
		int length = a_header->length;

		char buffer_recv[length]; //El buffer para recibir el mensaje se crea con la longitud recibida

		if (tipo == 121) {

			int bytes_recibidos = recv(KERNEL_DESCRIPTOR, buffer_recv,
					length, 0);

			t_PCB_serializacion *recibir_pcb = malloc(sizeof(t_PCB_serializacion));

			recibir_pcb = (t_PCB_serializacion * ) deserealizar_mensaje(121, buffer_recv);

			t_PCB *pcb = malloc(sizeof(t_PCB));

			//actualizarPCB(pcb, recibir_pcb);  -->Validar y agregar. Es un adapter
			pcb->instructions_index = recibir_pcb->instructions_index;
			pcb->pid = recibir_pcb->pid;
			pcb->instructions_size = recibir_pcb->instructions_size;
			pcb->program_counter = recibir_pcb->program_counter;
			pcb->stack = list_create();
			pcb->stack = recibir_pcb->stack_index;
			pcb->used_pages = recibir_pcb->used_pages;
			pcb->stack_size = recibir_pcb->stack_size;
			pcb->program_finished = recibir_pcb->program_finished;
			pcb->stack_free_space_pointer = malloc(sizeof(t_direccion_virtual_memoria));
			pcb->stack_free_space_pointer = recibir_pcb->stack_last_address;
			pcb->label_index = recibir_pcb->label_index;
			set_quantum(recibir_pcb->quantum);
			set_quantum_sleep(recibir_pcb->quantum_sleep);

			set_PCB(pcb);
			int resultado_ejecucion = ejecutar_pcb();
			if (pcb->program_finished == 5 )
				continue; // Se bloqueo por wait, no hay que mandar nada
			t_PCB_serializacion * pcb_serializado = adaptar_pcb_a_serializar(get_PCB());

			pcb_serializado->mensaje = 3;
			pcb_serializado->valor_mensaje = "";
			pcb_serializado->cantidad_operaciones = 0;
			pcb_serializado->valor_de_la_variable_compartida =0;
			pcb_serializado->resultado_mensaje = 0;
			pcb_serializado->cpu_unplugged = 0;
			if (pcb_serializado->program_finished == 6) {
				pcb_serializado->valor_mensaje = io_id;
				pcb_serializado->cantidad_operaciones = io_operations;
			}
			if (sigusr_received) {
				pcb_serializado->cpu_unplugged = 1;
			}
			t_stream * stream = serializar_mensaje(121,pcb_serializado);
			send(KERNEL_DESCRIPTOR, stream->datos, stream->size, 0);
			if (sigusr_received) {
				break;
			}
		}
	}

	return 0;
}

void connect_to_UMC() {

	int umc_socket_descriptor = create_client_socket_descriptor(umc_ip, umc_puerto);

	set_umc_socket_descriptor(umc_socket_descriptor);
	int a =1;
	send(umc_socket_descriptor, &a, sizeof(int), 0);
	int tamanio_pagina = -1;
	recv(umc_socket_descriptor, &tamanio_pagina, sizeof(int), 0);
	set_tamanio_pagina(tamanio_pagina);

}



void connect_to_Kernel() {

	int kernel_socket_descriptor =create_client_socket_descriptor(kernel_ip, kernel_puerto);

	set_kernel_socket_descriptor(kernel_socket_descriptor);
	int a =1;
	send(kernel_socket_descriptor, &a, sizeof(int), 0);

}


t_PCB_serializacion * adaptar_pcb_a_serializar(t_PCB * pcb){
	t_PCB_serializacion * pcb_serializacion = malloc(sizeof(t_PCB_serializacion));
	pcb_serializacion->instructions_index = pcb->instructions_index;
	pcb_serializacion->instructions_size = pcb->instructions_size;
	pcb_serializacion->label_index = pcb->label_index; //todo chequear que alan lo este inicializando
	pcb_serializacion->pid = pcb->pid;
	pcb_serializacion->program_counter = pcb->program_counter;
	pcb_serializacion->program_finished = pcb->program_finished;
	pcb_serializacion->quantum = 0;
	pcb_serializacion->quantum_sleep = 0;
	pcb_serializacion->stack_index = pcb->stack;
	pcb_serializacion->stack_last_address = pcb->stack_free_space_pointer;
	pcb_serializacion->stack_size = pcb->stack_size;
	pcb_serializacion->used_pages = pcb->used_pages;
	return pcb_serializacion;
}


void actualizarPCB(t_PCB *pcb, t_PCB_serializacion *recibir_pcb){
	pcb->instructions_index = recibir_pcb->instructions_index;
	pcb->pid = recibir_pcb->pid;
	pcb->instructions_size = recibir_pcb->instructions_size;
	pcb->program_counter = recibir_pcb->program_counter;
	pcb->stack = list_create();
	pcb->stack = recibir_pcb->stack_index;
	pcb->used_pages = recibir_pcb->used_pages;
	pcb->stack_size = recibir_pcb->stack_size;
	pcb->program_finished = recibir_pcb->program_finished;
	pcb->stack_free_space_pointer = malloc(sizeof(t_direccion_virtual_memoria));
	pcb->stack_free_space_pointer = recibir_pcb->stack_last_address;
	pcb->label_index = recibir_pcb->label_index;
	//TODO Agregar campos nuevos de la serializacion
}

void *captador_de_senal_thread(){

	//printf("Process ID: %d\n",getpid());
	fflush(stdout);

	if(signal(SIGUSR1,sig_handler) == SIG_ERR)
		error_show("No se atrapo la senal\n");

}

void sig_handler(){
	sigusr_received = 1;
};

int levanta_config_cpu(void){

	t_config *config_cpu = config_create("./config_cpu.txt");

	if(config_cpu==NULL){
		//TODO Loggear Error
		return 1;
	}

	kernel_ip = config_get_string_value(config_cpu,"IP_KERNEL");
	kernel_puerto = config_get_string_value(config_cpu,"PUERTO_KERNEL");
	umc_ip = config_get_string_value(config_cpu,"IP_UMC");
	umc_puerto = config_get_string_value(config_cpu,"PUERTO_UMC");

	return 0;
}






