/*
  nucleo.c
  Autor: Gustavo Boniscontro
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
#include "nucleo.h"
#include "kernel.h"
#include "protocoloKernel.h"

void set_umc_socket_descriptor(int socket){
	UMC_SOCKET_DESCRIPTOR = socket;
}
t_kernel* KERNEL;

void iniciar_algoritmo_planificacion() {

	KERNEL = create_kernel("./kernel_config.txt");

	//5 estados new, ready , exec, exit, block
	estado_new = queue_create();
	estado_ready = queue_create();
	estado_exit = queue_create();
	estado_block = queue_create();
	estado_ejecucion = queue_create();
	cola_cpu_disponibles =queue_create();

	pthread_t thReady, thNew, thEjecucion, thBlock,thExit;

	sem_init(&mut_new, 0, 1);
	sem_init(&mut_ready, 0, 1);
	sem_init(&mut_ejecucion, 0, 1);
	sem_init(&mut_block, 0, 1);
	sem_init(&mut_exit, 0, 1);
	sem_init(&mut_cpu_disponibles, 0, 1);


	sem_init(&cant_new, 0, 0);
	sem_init(&cant_ready, 0,0);
	sem_init(&cant_ejecucion, 0, 0);
	sem_init(&cant_block, 0, 0);
	sem_init(&cant_exit, 0, 0);
	sem_init(&cant_cpu_disponibles, 0, 0);


	 pthread_create(&thNew, NULL, &recNew, NULL);
	 pthread_create(&thReady, NULL, &recReady, NULL);
	 pthread_create(&thEjecucion, NULL, &recEjecucion, NULL);
	 pthread_create(&thBlock, NULL, &recBlock, NULL);
	 pthread_create(&thExit, NULL, &recExit, NULL);


//	 pthread_join(thNew, NULL);
//	 pthread_join(thReady, NULL);
//	 pthread_join(thEjecucion, NULL);
//	 pthread_join(thBlock, NULL);
//	 pthread_join(thExit, NULL);
//	 exit(EXIT_SUCCESS);




}




int iniciar_programa_en_umc(int pid, int cantidad_paginas_requeridas, char* codigo);
void *recNew() {


	while (1) {
		sem_wait(&cant_new);
		sem_wait(&mut_new);
		t_new_program * nuevo_programa = queue_pop(estado_new);
		sem_post(&mut_new);

		t_PCB *pcb = initialize_program(KERNEL,nuevo_programa->codigo_programa);
		pcb->console_socket_descriptor = nuevo_programa->console_socket_descriptor;
		int inicio_correcto = iniciar_programa_en_umc(pcb->pid, pcb->used_pages, nuevo_programa->codigo_programa);
		printf("resultado inicio programa en umc : %d\n", inicio_correcto); //TODO sacar este comentario
		fflush(stdout);

		sem_wait(&mut_ready);
		pcb->state = "Ready";
		queue_push(estado_ready, pcb);
		sem_post(&mut_ready);
		sem_post(&cant_ready);
	}

}

void *recReady() {


	while (1) {


		sem_wait(&cant_ready);
		sem_wait(&mut_ready);
		t_PCB *pcb  = queue_pop(estado_ready);
		sem_post(&mut_ready);


		sem_wait(&cant_cpu_disponibles); //espero tener una cpu disponible



		sem_wait(&mut_ejecucion);
		pcb->state = "Ejecutando";
		queue_push(estado_ejecucion, pcb);
		sem_post(&mut_ejecucion);
		sem_post(&cant_ejecucion);


	}

}

t_PCB_serializacion * adaptar_pcb_a_serializar(t_PCB * pcb){
	t_PCB_serializacion * pcb_serializacion = malloc(sizeof(t_PCB_serializacion));
	pcb_serializacion->instructions_index = pcb->instructions_index;
	pcb_serializacion->instructions_size = pcb->instructions_size;
	pcb_serializacion->label_index = pcb->label_index; //todo chequear que alan lo este inicializando
	pcb_serializacion->pid = pcb->pid;
	pcb_serializacion->program_counter = pcb->program_counter;
	pcb_serializacion->program_finished = 0; //TODO revisar que valor le pongo
	pcb_serializacion->quantum = KERNEL->quantum;
	pcb_serializacion->quantum_sleep = KERNEL->quantum_sleep;
	pcb_serializacion->stack_index = pcb->stack_index;
	pcb_serializacion->stack_last_address = pcb->stack_last_address;
	pcb_serializacion->stack_size = pcb->stack_size;
	pcb_serializacion->used_pages = pcb->used_pages;


	return pcb_serializacion;
}

void actualizar_pcb_serializado(t_PCB *pcb, t_PCB_serializacion *pcb_serializacion){
	pcb->instructions_index = pcb_serializacion->instructions_index;
	pcb->instructions_size = pcb_serializacion->instructions_size;
	pcb->label_index = pcb_serializacion->label_index;
	pcb->pid = pcb_serializacion->pid;
	pcb->program_counter = pcb_serializacion->program_counter;
	pcb->program_finished = pcb_serializacion->program_finished;
	pcb->stack_index = pcb_serializacion->stack_index;
	pcb->stack_last_address = pcb_serializacion->stack_last_address;
	pcb->stack_size = pcb_serializacion->stack_size;
	pcb->used_pages = pcb_serializacion->used_pages;

}

void ejecutar_pcb_en_cpu(t_PCB *pcb){

	t_PCB_serializacion * pcb_serializacion = adaptar_pcb_a_serializar(pcb);
	pcb_serializacion->mensaje = 0;
	pcb_serializacion->valor_mensaje = "";
	pcb_serializacion->cantidad_operaciones = 0;
	pcb_serializacion->resultado_mensaje = 0;
	t_stream *buffer = serializar_mensaje(121,pcb_serializacion);

	int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
	if (bytes_enviados == -1){
		printf("error al enviar pcb\n");
	}
	printf("pcb enviado a cpu \n ");
	while(1){

			t_header *un_header = malloc(sizeof(t_header));
			char buffer_header[5];

			int	bytes_recibidos_header,
				bytes_recibidos;

			bytes_recibidos_header = recv(pcb->cpu_socket_descriptor,buffer_header,5,MSG_PEEK);

			un_header = deserializar_header(buffer_header);

			uint8_t tipo = un_header->tipo;
			uint32_t length = un_header->length;


			char buffer_recibidos[length];

			if(tipo == 132){

				int bytes_recibidos = recv(pcb->cpu_socket_descriptor,buffer_recibidos,length,0);


				int bytes_sent = send(pcb->console_socket_descriptor,buffer_recibidos,length,0);
				printf("Envio imprimir texto a consola\n");
			}
			if(tipo == 121){

				int bytes_recibidos = recv(pcb->cpu_socket_descriptor,buffer_recibidos,length,0);
				t_PCB_serializacion *unPCB = deserealizar_mensaje(121,buffer_recibidos);

				actualizar_pcb_serializado(pcb, unPCB);

				if(unPCB->mensaje == 1){
					uint32_t valor_variable =get_shared_var_value(KERNEL, unPCB->valor_mensaje);

					t_PCB_serializacion * pcb_serializacion = adaptar_pcb_a_serializar(pcb);
					pcb_serializacion->mensaje = 0;
					pcb_serializacion->valor_mensaje = "";
					pcb_serializacion->cantidad_operaciones = 0;
					pcb_serializacion->resultado_mensaje = valor_variable;
					t_stream *buffer = serializar_mensaje(121,pcb_serializacion);

					int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
					if (bytes_enviados == -1){
						printf("error al enviar pcb\n");
					}



				}else if(unPCB->mensaje ==2){
					//aca hay que renombrar el cantidad de operaciones ya que no imagine todos los casos.
					//estoy reutilizadno el campo para no serializar algo mas
					uint32_t resultado =update_shared_var_value(KERNEL, unPCB->valor_mensaje, unPCB->cantidad_operaciones);
					t_PCB_serializacion * pcb_serializacion = adaptar_pcb_a_serializar(pcb);
					pcb_serializacion->mensaje = 0;
					pcb_serializacion->valor_mensaje = "";
					pcb_serializacion->cantidad_operaciones = 0;
					pcb_serializacion->resultado_mensaje = resultado;
					t_stream *buffer = serializar_mensaje(121,pcb_serializacion);

					int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
					if (bytes_enviados == -1){
						printf("error al enviar pcb\n");
					}


				}

			}
	}
}

void * recEjecucion() {

	while(1){

		sem_wait(&cant_ejecucion);
		sem_wait(&mut_ejecucion);
		t_PCB *pcb  = queue_pop(estado_ejecucion);
		sem_post(&mut_ejecucion);


		sem_wait(&mut_cpu_disponibles);
		int cpu  = (int )queue_pop(cola_cpu_disponibles);
		sem_post(&mut_cpu_disponibles);


		pcb->cpu_socket_descriptor = cpu;
		pthread_t th_ejecucion_pcb;
		pthread_create(&th_ejecucion_pcb, NULL, &ejecutar_pcb_en_cpu, pcb);

	}
}



int finalizar_programa_consola(t_PCB *pcb){
	t_finalizar_programa_en_consola * finalizar_consola = malloc(sizeof(t_finalizar_programa_en_consola));
	memset(finalizar_consola,0,sizeof(t_finalizar_programa_en_consola));

	finalizar_consola->motivo = 0;
	t_stream *buffer = malloc(sizeof(t_stream));

	buffer = serializar_mensaje(133,finalizar_consola);

	int bytes_enviados = send(pcb->console_socket_descriptor,buffer->datos,buffer->size,0);

	return bytes_enviados;
}

int finalizar_programa_umc(t_PCB *pcb){
		t_finalizar_programa_en_UMC *finalizar_programa_en_UMC = malloc(sizeof(t_finalizar_programa_en_UMC));
		memset(finalizar_programa_en_UMC,0,sizeof(t_finalizar_programa_en_UMC));



		finalizar_programa_en_UMC->process_id = pcb->pid;

		t_stream *buffer = malloc(sizeof(t_stream));

		buffer = serializar_mensaje(63,finalizar_programa_en_UMC);

		int bytes_enviados = send(UMC_SOCKET_DESCRIPTOR,buffer->datos,buffer->size,0);

		char buffer_header[5];

		int bytes_header = recv(UMC_SOCKET_DESCRIPTOR,buffer_header,5,MSG_PEEK);

		char buffer_recv[buffer_header[1]];

		int bytes_recibidos = recv(UMC_SOCKET_DESCRIPTOR,buffer_recv,buffer_header[1],0);

		t_respuesta_finalizar_programa_en_UMC *respuesta_finalizar_prog_UMC = malloc(sizeof(t_respuesta_finalizar_programa_en_UMC));

		memset(respuesta_finalizar_prog_UMC,0,sizeof(t_respuesta_finalizar_programa_en_UMC));

		respuesta_finalizar_prog_UMC = deserealizar_mensaje(64,buffer_recv);

		return respuesta_finalizar_prog_UMC->respuesta_correcta;
}

void * recBlock(){
	while(1){

	}
}
void *recExit() {

	while (1) {
			sem_wait(&cant_exit);
			sem_wait(&mut_exit);
			t_PCB *pcb = queue_pop(estado_exit);
			sem_post(&mut_exit);

			int umc_finalizado =finalizar_programa_umc(pcb);
			int consola_finalizado =finalizar_programa_consola(pcb);

			free(pcb);// lo libero directamente creo q no es necesario hacer cola de exit


	}
}


int obtener_cantidad_paginas_programa(t_metadata_program* metadata, int bytes_por_pagina) {
	int bytes_totales = 0;
	int i;
	t_intructions* array_instrucciones = metadata->instrucciones_serializado;
	for(i=0; i < metadata->instrucciones_size; i++) {
		bytes_totales += (array_instrucciones->offset);
		array_instrucciones++;
	}

	int total_paginas = bytes_totales / bytes_por_pagina;

	if(total_paginas * bytes_por_pagina < bytes_totales) {
		total_paginas += 1;
	}

	return total_paginas;
}


int iniciar_programa_en_umc(int pid, int cantidad_paginas_requeridas, char* codigo){

	   t_inicio_de_programa_en_UMC *iniciar_programa_en_UMC = malloc(sizeof(t_inicio_de_programa_en_UMC));
	   memset(iniciar_programa_en_UMC,0,sizeof(t_inicio_de_programa_en_UMC));

	   iniciar_programa_en_UMC->process_id = pid;
	   iniciar_programa_en_UMC->cantidad_de_paginas = cantidad_paginas_requeridas;
	   iniciar_programa_en_UMC->codigo_de_programa = codigo;

	   t_stream *buffer = malloc(sizeof(t_stream));

	   buffer = serializar_mensaje(61,iniciar_programa_en_UMC);

	   int bytes_enviados = send(UMC_SOCKET_DESCRIPTOR,buffer->datos,buffer->size,0);

	   char buffer_header[5];

	   int bytes_header = recv(UMC_SOCKET_DESCRIPTOR,buffer_header,5,MSG_PEEK);

	   char buffer_recv[buffer_header[1]];

	   int bytes_recibidos = recv(UMC_SOCKET_DESCRIPTOR,buffer_recv,buffer_header[1],0);

	   t_respuesta_iniciar_programa_en_UMC *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_UMC));
	   memset(respuesta,0,sizeof(t_respuesta_iniciar_programa_en_UMC));
	   respuesta = deserealizar_mensaje(buffer_header[0],buffer_recv);

	   return respuesta->respuesta_correcta;


}
