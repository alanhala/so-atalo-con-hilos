#include "nucleo.h"



void Planificacion() {

	//5 estados new, ready , exec, exit, block
	pColaNew = queue_create();
	pColaReady = queue_create();
	pColaExit = queue_create();
	pColaBlock = queue_create();
  pListaCpu = list_create();
	pthread_t thReady, thNew, thExec, thBlock,thExit;

	sem_init(&mut_new, 0, 1);
	sem_init(&mut_ready, 0, 1);
	sem_init(&mut_cpu, 0, 1);
	sem_init(&mut_block, 0, 1);
	sem_init(&mut_exit, 0, 1);

	sem_init(&cant_new, 0, 0);
	sem_init(&cant_ready, 0,0);
	sem_init(&cant_cpu, 0, 0);
	sem_init(&cant_block, 0, 0);
	sem_init(&cant_exit, 0, 0);

	/* COMENTO PARA TEST, DESCOMENTAR*/

	 pthread_create(&thNew, NULL, &recNew, NULL);
	 pthread_create(&thReady, NULL, &recReady, NULL);
	 pthread_create(&thExec, NULL, &recExec, NULL);
	 pthread_create(&thExit, NULL, &recExit, NULL);

	 printf("Hilo creado \n"); //TODO BORRAR LINEA

	 pthread_join(thNew, NULL);
	 pthread_join(thReady, NULL);
	 pthread_join(thExec, NULL);
	 //  pthread_join( thBlock, NULL);
	 exit(EXIT_SUCCESS);


	 /*COMENTO PARA TEST, DESCOMENTAR */

}



t_PCB *conectarConsola()
{
	//aca falta conectarse con la consola y crear un nuevo PCB
		//yo creo uno con solo un id para prueba
		//int client_socket_descriptor = accept_connection(socket_desciptor);

	  t_PCB *pcb;
    pcb = malloc(sizeof(t_PCB));
    pcb->pid = pid++ ;
    pcb->program_counter = 0;
    return(pcb);
}

t_PCB *createPCB()
{
	  t_PCB *pcb;
    pcb = malloc(sizeof(t_PCB));
    pcb->pid = pid++ ;
    pcb->program_counter = 0;
    return(pcb);
}

void *recNew() {
t_PCB *pcb;

	while (1) {
		pcb = conectarConsola();
    sem_wait(&mut_new);
    pcb->estado=NEW;
		queue_push(pColaNew,pcb);
		sem_post(&mut_new);
    sem_post(&cant_new);

		sleep(2);   //este sleep para q 1 seg me crea proceso
	}

}




void *recReady() {
t_PCB *pcb;

	while (1) {
		sem_wait(&cant_new);
		sem_wait(&mut_new);
		pcb = queue_pop(pColaNew);
		sem_post(&mut_new);
		sem_wait(&mut_ready);
		pcb->estado = READY;
		queue_push(pColaReady, pcb);
		sem_post(&mut_ready);
		sem_post(&cant_ready);

		sleep(1);
	}

}
int conectarCPU(){
  //TODO conectar con una cpu
  //falta hacer este codigo
  //aca tendria que

  sem_post(&cant_cpu);

}

void deReadyaExec(t_cpu *auxcpu ) {
t_PCB *pcb;
int idcpu ;
 			sem_wait(&cant_cpu); //primero vemos que haya alguna cpu libre
			sem_wait(&cant_ready);

			sem_wait(&mut_ready);
			pcb = queue_pop(pColaReady);
			sem_post(&mut_ready);


			sem_wait(&mut_cpu);
      pcb->estado=EXEC;
			//list_add_in_index(t_list *self, int index, void *data)
      list_add_in_index(pListaCpu,idcpu, pcb);
			sem_post(&mut_cpu);
}




t_cpu *mensajeCPU()
{
    t_cpu *auxcpu = malloc(sizeof(t_msjcpu));
    auxcpu->msj = (rand() % 4); //TODO RECIBIR POR SOCKET MSJ
    auxcpu->id=1; //TODO recibir id de cpu;
    //auxcpu->pcb =
    return (auxcpu);

}


void *recExec() {
t_PCB *pcb;
int idcpu ;
t_cpu *auxcpu;
t_cpu *cpu_in_list;
	while (1) {
	   auxcpu = mensajeCPU();
     switch(auxcpu->msj) {
        case CPU_IDLE :
           sem_post(&cant_cpu);
           deReadyaExec(auxcpu);
           break;
        case CPU_PREEMPT :
            sem_wait(&mut_cpu);
            cpu_in_list = list_get(pListaCpu,auxcpu->id);
            sem_post(&mut_cpu);
            pcb = cpu_in_list->pcb;
        		sem_wait(&mut_ready);
        		pcb->estado = READY;
        		queue_push(pColaReady, pcb);
        		sem_post(&mut_ready);
        		sem_post(&cant_ready);
            sem_post(&cant_cpu);
           break;
        case CPU_BLOCK :
            sem_wait(&mut_cpu);
            cpu_in_list = list_get(pListaCpu,auxcpu->id);
            sem_post(&mut_cpu);
            pcb = cpu_in_list->pcb;
        		sem_wait(&mut_block);
        		pcb->estado = BLOCK;
        		queue_push(pColaBlock, pcb);
        		sem_post(&mut_block);
        		sem_post(&cant_block);
           break;
        case CPU_EXIT :
           //aca hay que liberar la cpu ponerla en idle y el proceso poner en cola exit
            sem_wait(&mut_cpu);
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
        		sem_post(&cant_exit);

           break;
      }

	}
}

void *recExit() {
t_PCB *pcb;
	while (1) {
			sem_wait(&cant_exit);
			sem_wait(&mut_exit);
			pcb = queue_pop(pColaExit);
			sem_post(&mut_exit);
		  //TODO llamar a consola y enviar pcb

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

