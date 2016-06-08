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
#include "protocoloKernel.h"

void set_umc_socket_descriptor(int socket){
	UMC_SOCKET_DESCRIPTOR = socket;
}


void iniciar_algoritmo_planificacion() {

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



t_PCB *createPCB(char *codigo_programa)
{
    t_PCB *pcb;
    pcb = malloc(sizeof(t_PCB));
    pcb->pid = 10 ;
    pcb->program_counter = 0;
    pcb->codigo_programa = codigo_programa;
    pcb->paginas_codigo = 50;
	return(pcb);
}

int iniciar_programa_en_umc(int pid, int cantidad_paginas_requeridas, char* codigo);
void *recNew() {


	while (1) {
		sem_wait(&cant_new);
		sem_wait(&mut_new);
		char * codigo_programa = queue_pop(estado_new);
		sem_post(&mut_new);

		t_PCB *pcb = createPCB(codigo_programa);

		int inicio_correcto = iniciar_programa_en_umc(pcb->pid, pcb->paginas_codigo, pcb->codigo_programa);
		printf("resultado inicio programa en umc : %d", inicio_correcto); //TODO sacar este comentario
		fflush(stdout);
		sem_wait(&mut_ready);
		pcb->estado = READY;
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
		sem_post(&cant_cpu_disponibles);


		sem_wait(&mut_ejecucion);
		pcb->estado = EXEC;
		queue_push(estado_ejecucion, pcb);
		sem_post(&mut_ejecucion);
		sem_post(&cant_ejecucion);





	}

}



void * ejecutar_pcb_en_cpu(t_cpu *cpu){

}

void * recEjecucion() {
	while(1){
		sem_wait(&cant_cpu_disponibles);
		sem_wait(&mut_cpu_disponibles);
		t_cpu *cpu  = queue_pop(cola_cpu_disponibles);
		sem_post(&mut_cpu_disponibles);


		sem_wait(&cant_ejecucion);
		sem_wait(&mut_ejecucion);
		t_PCB *pcb  = queue_pop(estado_ejecucion);
		sem_post(&mut_ejecucion);

		cpu->pcb =pcb;
		pthread_t th_ejecucion_pcb;
		pthread_create(&th_ejecucion_pcb, NULL, &ejecutar_pcb_en_cpu, cpu);

	}




}



int finalizar_programa_consola(t_PCB *pcb){

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

int atender_mensaje_cpu(int mensaje,char *identificador,char *valor)
{
/*
al llamar en caso de que los valores no sean necesarios llenar con null
obtener_valor [identificador de variable compartida]
grabar_valor [identificador de variable compartida] [valor a grabar]
wait [identificador de semáforo]
signal [identificador de semáforo]
entrada_salida [identificador de dispositivo] [unidades de tiempo a utilizar]
*/
     switch(mensaje) {
        case CPU_IDLE :
          // sem_post(&cant_cpu);
           //deReadyaExec(auxcpu);
           break;
        case fin_quantum :
           /* sem_wait(&mut_cpu);
            cpu_in_list = list_get(pListaCpu,auxcpu->id);
            sem_post(&mut_cpu);
            pcb = cpu_in_list->pcb;
        		sem_wait(&mut_ready);
        		pcb->estado = READY;
        		queue_push(pColaReady, pcb);
        		sem_post(&mut_ready);
        		sem_post(&cant_ready);
            sem_post(&cant_cpu);    */
           break;
        case entrada_salida :
        	//todo necesito tener creada la listadispositivos desde el archivo de configuracion
        	extern t_list *listadispositivos ;
        	t_entradasalida *io;
        	io = list_find(listadispositivos,strcmp(this->dispositivo,identificador));
        	queue_push(io->cola,pcb);
        	//cada pcb se pone en cada cola de cada dispositivo

           /* sem_wait(&mut_cpu);
            cpu_in_list = list_get(pListaCpu,auxcpu->id);
            sem_post(&mut_cpu);
            pcb = cpu_in_list->pcb;
        		sem_wait(&mut_block);
        		pcb->estado = BLOCK;
        		queue_push(pColaBlock, pcb);
        		sem_post(&mut_block);
        		sem_post(&cant_block);  */
           break;
        case fin_proceso :
           //aca hay que liberar la cpu ponerla en idle y el proceso poner en cola exit
           /* sem_wait(&mut_cpu);
            cpu_in_list = list_get(pListaCpu,auxcpu->id);
            cpu_in_list->msj = CPU_IDLE; //TODO este elemento se modifica directamente en la listacpu?? nose
            //capaz hay que hacer
            //list_add_in_index(cpu_in_list,auxcpu->id, cpu_in_list);
            pcb = cpu_in_list->pcb;
            sem_post(&mut_cpu);
            sem_post(&cant_cpu);

        		sem_wait(&mut_exit);
        		pcb->estado = EXIT;
        		queue_push(pColaExit, pcb);
        		sem_post(&mut_exit);
        		sem_post(&cant_exit);    */
           break;
        case obtener_valor:
        //obtener_valor [identificador de variable compartida]
        	break;
		case grabar_valor:
		//grabar_valor [identificador de variable compartida] [valor a grabar]
			break;
		case cpuwait:
		//wait [identificador de semáforo]
			break;
		case cpusignal:
		//signal [identificador de semáforo]
			break;
      }

}





