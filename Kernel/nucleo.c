#include <commons/collections/queue.h>
#include <stdio.h>
#include <stdlib.h>


struct PCB {
    int pid;
    int rafaga;
    int in;
 };

struct PCB *CreatePCB ();





/*int main (void) {

	int i,q,tact;
    struct PCB  *ptmp;

    void *pColaNew,*pColaReady, *pColaExit, *pColaBlock,*pColaExec,*pColaPro;

    //5 estados new, ready , exec, exit, block
    pColaPro=queue_create();
    pColaNew=queue_create();
    pColaReady=queue_create();
    pColaExit=queue_create();
    pColaBlock=queue_create();
    pColaExec=queue_create();
    //test
    //trate de hacer el ejercicio de round robin explicado en clase
    //en el caso de los io lo puse como que volvia a retomar el proceso un tiempo dsp segun el de ej de clase

    int vn[] =  {1,3,3,2,4,1,2};//id proceso
    int vin[] =  {0,1,10,10,11,13,21};//tiempo de llegada
    int vra[] =  {9,2,1,5,1,2,3};//duracion


    for (i=0;i<=6;i++){
    	queue_push(pColaPro,CreatePCB(vn[i],vin[i],vra[i]));
    }
    q = 4;
    //fin test

    tact =0;
    int taux = 0;
    //Comienza algoritmo round robin
    //para que funcione los tiempo de llegada tienen q estar ordenados en la cola
    while (!queue_is_empty(pColaPro) || !queue_is_empty(pColaNew) || !queue_is_empty(pColaReady) || !queue_is_empty(pColaBlock)  || !queue_is_empty(pColaExec)  )
    {//mientras haya procesos en alguna cola
    	taux = tact;

    	while (!queue_is_empty(pColaPro) &&  (ptmp=queue_peek(pColaPro))->in < tact ){

				ptmp = queue_pop(pColaPro);
				queue_push(pColaNew,ptmp);
		}
    	while (!queue_is_empty(pColaNew) ){

				ptmp = queue_pop(pColaNew);
				queue_push(pColaReady,ptmp);
		}
    	while (!queue_is_empty(pColaExec) ){//los que terminaro de ejecutarse

				ptmp = queue_pop(pColaExec);
				queue_push(pColaReady,ptmp);
		}

    	//esto es para que los que acaban de entrar no tengan mas prioridad de los que se estan ejecutando
    	while (!queue_is_empty(pColaPro) &&  (ptmp=queue_peek(pColaPro))->in == tact ){

				ptmp = queue_pop(pColaPro);
				queue_push(pColaNew,ptmp);
		}
    	while (!queue_is_empty(pColaNew) ){

				ptmp = queue_pop(pColaNew);
				queue_push(pColaReady,ptmp);
		}
    	//fin prioridad los que acaban de entrar


		while (!queue_is_empty(pColaReady)){
			ptmp = queue_pop(pColaReady);

			if ( q < ptmp->rafaga ){


				tact += q ;
				ptmp->rafaga = ptmp->rafaga-q;
				queue_push(pColaExec,ptmp);// los pcb que se ejecutan
				printf("Ejecutando proceso %d tiempo cpu %d\n",ptmp->pid,tact);

			}
			else{
				tact += ptmp->rafaga;
				queue_push(pColaExit,ptmp);
				printf("Termina   proceso %d tiempo cpu %d\n",ptmp->pid,tact);

			}


		}//fin ready

    	if (tact == taux){
    		//por lo tanto hay un idle no se ejecuto nada
    		tact++;
    	}

    }
   	while (!queue_is_empty(pColaExit)){
				ptmp = queue_pop(pColaExit);
				free(ptmp);
		}
    ///free(ptmp); acordarse de eliminar los punteros
    return(0);
};*/

struct PCB *CreatePCB (int pid,int in,int raf) {
    struct PCB *proc;
    proc = malloc(sizeof(struct PCB));
    if (proc == NULL) {
        printf("ERROR");
        exit(1);
    };
    proc->pid = pid;
    proc->rafaga = raf;
    proc->in = in;
    return(proc);
};


