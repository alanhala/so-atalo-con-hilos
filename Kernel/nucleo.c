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
	//TODO aca falta conectarse con la consola y crear un nuevo PCB
		//yo creo uno con solo un id para prueba
		//int client_socket_descriptor = accept_connection(socket_desciptor);

	t_PCB *pcb;
	pcb = malloc(sizeof(t_PCB));
    if (pcb == NULL) {
        printf("ERROR");
        exit(1);
    };
    pcb->pid = pid++;
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

void *recExec() {
t_PCB *pcb;
int idcpu ;
	while (1) {
	    idcpu = conectarCPU();
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
}
void *recExit() {
	while (1) {
		/*	sem_wait(&cant_exec);
			sem_wait(&mut_exec);
			ptmp = queue_pop(pColaExec);
			sem_post(&mut_exec);
			printf("Exit  proceso %d tiempo cpu \n",ptmp->pid);

			free(ptmp);// lo libero directamente creo q no es necesario hacer cola de exit
			sem_post(&cant_cpu); // libre una cpu para que pueda seguir procesando
     */

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

