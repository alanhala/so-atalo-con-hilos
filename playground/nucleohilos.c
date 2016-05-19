#include <commons/collections/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

struct CPU {
	int id;
	int speed;
};

struct PCB {
	int pid;
	struct CPU *cpuexec;
	int remaing;
	int wtime;
	int in;
	int rafaga;
	int io;
	int r2;
};

static sem_t mut_n, mut_r, mut_e, mut_b, mut_s, mut_c;
static int i, q, tact;

static int qt;
static struct PCB *ptmp;
static struct CPU *pcpu;

static void *pColaNew, *pColaReady, *pColaExit, *pColaBlock, *pColaExec,
		*pColaPro, *pColaCpu;

//pthread_cond_t count_waitexample;

struct PCB *CreatePCB();
struct CPU *CreateCPU(int id, int speed);
void *recNew();
void *recReady();
void *recBlock();
void *recExec();
void *Planificacion();
void *TestSemaforos();

void *agregarAColaTest() {
	sem_wait(&mut_n);
	queue_push(pColaNew, ptmp);
	sem_post(&mut_n);
}
void *sacarDeColaTest() {
	sem_wait(&mut_n);
	ptmp = queue_pop(pColaNew);
	sem_post(&mut_n);
}
/*
int main(void) {

	//5 estados new, ready , exec, exit, block

	pthread_t planif_thread, testSemaforos;
	pthread_create(&planif_thread, NULL, &Planificacion, NULL);

	pthread_create(&testSemaforos, NULL, &TestSemaforos, NULL);

	while (1) {
	}
	return 0;

}*/

void *TestSemaforos() {

	int a;
	for (a = 0; a < 100000; a = a + 1) {
		pthread_t agregarAColaThread, sacarDeColaThread;
		pthread_create(&agregarAColaThread, NULL, &agregarAColaTest, NULL);
		pthread_create(&sacarDeColaThread, NULL, &sacarDeColaTest, NULL);

		//sem_wait(&mut_r);
		//queue_push(pColaReady, ptmp);
		//sem_post(&mut_r);
	}
	int size = queue_size(pColaNew);
	if (!size) {
		printf("colaVacia");
		fflush(stdout);
	} else {
		printf("cola con %d elementos \n", size);
		fflush(stdout);
	}
	sleep(10);
}

void *Planificacion() {
	//5 estados new, ready , exec, exit, block
	pColaPro = queue_create();
	pColaNew = queue_create();
	pColaReady = queue_create();
	pColaExit = queue_create();
	pColaBlock = queue_create();
	pColaExec = queue_create();
	pColaCpu = queue_create();
	pthread_t thReady, thNew, thExec, thBlock;
	//pthread_cond_init (&count_threshold_cv, NULL);
	sem_init(&mut_r, 0, 1);
	sem_init(&mut_e, 0, 1);
	sem_init(&mut_b, 0, 1);
	sem_init(&mut_s, 0, 1);
	sem_init(&mut_c, 0, 1);

	/* COMENTO PARA TEST, DESCOMENTAR

	 int iret1 = pthread_create(&thNew, NULL, &recNew, NULL);
	 int iret2 = pthread_create(&thReady, NULL, &recReady, NULL);
	 int iret3 = pthread_create(&thExec, NULL, &recExec, NULL);
	 int iret4 = pthread_create(&thBlock, NULL, &recBlock, NULL);
	 //Cuarto Parametro:
	 //*arg - pointer to argument of function.
	 //To pass multiple arguments, send a pointer to a structure.
	 if (iret1 || iret2 || iret3 || iret4) {
	 printf("Error al crear los hilos ");
	 exit(1);
	 }
	 printf("Hilo creado \n"); //TODO BORRAR LINEA
	 //pthread_join(thread, NULL);
	 pthread_join(thNew, NULL);
	 pthread_join(thReady, NULL);
	 pthread_join(thExec, NULL);
	 //  pthread_join( thBlock, NULL);
	 exit(EXIT_SUCCESS);


	 COMENTO PARA TEST, DESCOMENTAR */

}

void *recReady() {

	while (1) {
		sem_wait(&mut_n);
		ptmp = queue_pop(pColaNew);
		sem_post(&mut_n);
		//printf("Ready  proceso %d tiempo cpu %d\n",ptmp->pid,tact);
		sem_wait(&mut_r);
		queue_push(pColaReady, ptmp);
		sem_post(&mut_r);

		sleep(1);
	}

	//TODO CERRAR CONEXION DEL SOCKET
}

void *recNew() {

	//TODO que reciba el socket_desciptor
	while (1) {

		//TODO int client_socket_descriptor = accept_connection(socket_desciptor);
		sem_wait(&mut_n);
		queue_push(pColaNew, CreatePCB()); //
		sem_post(&mut_n);
		printf("Se conectio una consola");
		fflush(stdout);
	}
	//TODO CERRAR CONEXION DEL SOCKET
}

void *recExec() {
	while (1) {
		if (!queue_is_empty(pColaReady) && !queue_is_empty(pColaCpu)) {

			sem_wait(&mut_r);
			ptmp = queue_pop(pColaReady);
			sem_post(&mut_r);
			sem_wait(&mut_c);
			pcpu = queue_pop(pColaCpu);
			sem_post(&mut_c);
			ptmp->cpuexec = pcpu;

			//printf("Exec  proceso %d tiempo cpu %d\n",ptmp->pid,tact);
			sem_wait(&mut_e);
			queue_push(pColaExec, ptmp);
			sem_post(&mut_e);
		}
	}
}
void *recBlock() {
	while (1) {
		if (!queue_is_empty(pColaBlock)) {
			ptmp = queue_pop(pColaBlock);
			ptmp->io = ptmp->io - 1;
			if (ptmp->io > 0) {
				queue_push(pColaBlock, ptmp);
			} else {
				if (ptmp->rafaga > 0) {
					//printf("readyblock proceso %d tiempo cpu %d\n",ptmp->pid,tact);
					queue_push(pColaReady, ptmp);
				} else {
					//ptmp->remaing = tact;
					//printf("Termina   proceso %d tiempo cpu %d\n",ptmp->pid,tact);
					queue_push(pColaExit, ptmp);
				}
			}
		}
	}
}

/*
 int simuladorr3(void) {

 //5 estados new, ready , exec, exit, block
 pColaPro = queue_create();
 pColaNew = queue_create();
 pColaReady = queue_create();
 pColaExit = queue_create();
 pColaBlock = queue_create();
 pColaExec = queue_create();
 pColaCpu = queue_create();
 //test
 //trate de hacer el ejercicio de round robin explicado en clase
 //en el caso de los io lo puse como que volvia a retomar el proceso un tiempo dsp segun el de ej de clase

 int vn[] = { 0, 2, 1, 3 };    //id proceso
 int vin[] = { 0, 1, 10, 11 };    //tiempo de llegada
 int vra[] = { 9, 2, 5, 1 };    //duracion
 int vra2[] = { 2, 1, 3, 0 }; //rafa 2
 int vio[] = { 2, 4, 1, 0 }; //i/o time

 for (i = 0; i <= 3; i++) {
 //TODO NO EXISTE LA FUNCION	queue_push(pColaPro, CreatePCB(vn[i], vin[i], vra[i], vio[i], vra2[i]));
 }
 for (i = 1; i <= 1; i++) {
 //TODO NO EXISTE LA FUNCION	queue_push(pColaCpu, CreateCPU(i, 1));
 }
 q = 4;
 qt = q;

 //fin test

 tact = 0;

 //Comienza algoritmo round robin
 //para que funcione los tiempo de llegada tienen q estar ordenados en la cola

 while (!queue_is_empty(pColaPro)
 && (ptmp = queue_peek(pColaPro))->in <= tact) {

 ptmp = queue_pop(pColaPro);
 printf("Saco  proceso %d tiempo cpu %d\n", ptmp->pid, tact);
 queue_push(pColaNew, ptmp);
 }

 while (!queue_is_empty(pColaPro) || !queue_is_empty(pColaNew)
 || !queue_is_empty(pColaReady) || !queue_is_empty(pColaBlock)
 || !queue_is_empty(pColaExec)) { //mientras haya procesos en alguna cola

 if (qt == 0)
 qt = q;

 while (!queue_is_empty(pColaPro)
 && (ptmp = queue_peek(pColaPro))->in <= tact) {

 ptmp = queue_pop(pColaPro);
 printf("Saco  proceso %d tiempo cpu %d\n", ptmp->pid, tact);
 queue_push(pColaNew, ptmp);
 }

 if (!queue_is_empty(pColaNew)) {

 ptmp = queue_pop(pColaNew);
 printf("Ready  proceso %d tiempo cpu %d\n", ptmp->pid, tact);
 queue_push(pColaReady, ptmp);
 }
 //fin prioridad los que acaban de entrar

 while (!queue_is_empty(pColaReady) && !queue_is_empty(pColaCpu)) {
 ptmp = queue_pop(pColaReady);
 pcpu = queue_pop(pColaCpu);
 ptmp->cpuexec = pcpu;
 printf("Exec  proceso %d tiempo cpu %d\n", ptmp->pid, tact);
 queue_push(pColaExec, ptmp);
 }

 if (!queue_is_empty(pColaExec)) {

 qt = qt - 1;
 ptmp = queue_pop(pColaExec);
 if (ptmp->rafaga > 0) {
 ptmp->rafaga = ptmp->rafaga - 1;
 }

 if (qt == 0) {
 if (ptmp->rafaga == 0) {
 if (ptmp->io > 0) {
 ptmp->io++; // le sumo uno porque abajo se lo saco y se lo tengo q sacar en el prox ciclo
 ptmp->rafaga = ptmp->r2;
 ptmp->r2 = 0;
 printf("Block proceso %d tiempo cpu %d\n", ptmp->pid,
 tact);
 queue_push(pColaCpu, ptmp->cpuexec);
 queue_push(pColaBlock, ptmp);
 } else {
 ptmp->remaing = tact;
 printf("Termina   proceso %d tiempo cpu %d\n",
 ptmp->pid, tact);
 queue_push(pColaCpu, ptmp->cpuexec);
 queue_push(pColaExit, ptmp);
 }
 } else {

 printf("%d libero x q proceso %d tiempo cpu %d\n", qt,
 ptmp->pid, tact);
 queue_push(pColaCpu, ptmp->cpuexec);
 queue_push(pColaReady, ptmp);
 }

 }

 if (qt > 0) {
 if (ptmp->rafaga == 0) {
 qt = q;
 if (ptmp->io > 0) {
 ptmp->io++; // le sumo uno porque abajo se lo saco y se lo tengo q sacar en el prox ciclo
 ptmp->rafaga = ptmp->r2;
 ptmp->r2 = 0;
 printf("Block proceso %d tiempo cpu %d\n", ptmp->pid,
 tact);
 queue_push(pColaCpu, ptmp->cpuexec);
 queue_push(pColaBlock, ptmp);
 } else {
 ptmp->remaing = tact;
 printf("Termina   proceso %d tiempo cpu %d\n",
 ptmp->pid, tact);
 queue_push(pColaCpu, ptmp->cpuexec);
 queue_push(pColaExit, ptmp);
 }
 } else {
 //printf("%d libero x q proceso %d tiempo cpu %d\n",qt,ptmp->pid,tact);
 queue_push(pColaExec, ptmp);
 }
 }
 }

 if (!queue_is_empty(pColaBlock)) {
 ptmp = queue_pop(pColaBlock);
 ptmp->io = ptmp->io - 1;
 if (ptmp->io > 0) {
 queue_push(pColaBlock, ptmp);
 } else {
 if (ptmp->rafaga > 0) {
 printf("readyblock proceso %d tiempo cpu %d\n", ptmp->pid,
 tact);
 queue_push(pColaReady, ptmp);
 } else {
 ptmp->remaing = tact;
 printf("Termina   proceso %d tiempo cpu %d\n", ptmp->pid,
 tact);
 queue_push(pColaExit, ptmp);
 }
 }
 }

 tact++;

 }

 while (!queue_is_empty(pColaExit)) {
 ptmp = queue_pop(pColaExit);

 free(ptmp);
 }
 ///free(ptmp); acordarse de eliminar los punteros
 return (0);
 };
 */
struct PCB *CreatePCB() {
	struct PCB new_pcb;

	struct PCB * ptr_new_pcb = &new_pcb;

	return ptr_new_pcb;
}

