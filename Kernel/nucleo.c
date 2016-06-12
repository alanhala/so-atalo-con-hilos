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



void ejecutar_pcb_en_cpu(t_PCB *pcb){


	t_stream *buffer = serializar_mensaje(121,pcb);

	int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
	if (bytes_enviados == -1){
		printf("error al enviar pcb\n");
	}
	printf("pcb enviado a cpu \n ");
	while(1){



	}
	//while(1){
		//aca tengo que hacer un while de todas las respuestas que me va mandando cpu
		// entre ellas las de semaforos, dispositivos, terminar programa y termino ejecucion de
		//quantum
	//}

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
	return -1;
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
	   //char unChar[5] = "Hola";
	   //memcpy(iniciar_programa_en_UMC->codigo_de_programa,&unChar,5);

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
