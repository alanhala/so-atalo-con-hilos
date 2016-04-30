#include <commons/collections/queue.h>
#include <stdio.h>
#include <stdlib.h>

struct CPU {
    int id;
    int speed;
 };

struct PCB {
    int pid;
    struct CPU  *cpuexec;
    int remaing;
    int wtime;
    int in;
    int rafaga;
    int io;
    int r2;
 };



struct PCB *CreatePCB ();
struct CPU *CreateCPU (int id,int speed) ;

int main2(void){
	simuladorr2();
	return 0;
}

int simuladorr2 (void) {

	int i=0,q=0,tact=0;

    int qt=0;
    struct PCB  *ptmp;
    struct CPU  *pcpu;

    void *pColaNew,*pColaReady, *pColaExit, *pColaBlock,*pColaExec,*pColaPro, *pColaCpu;

    //5 estados new, ready , exec, exit, block
    pColaPro=queue_create();
    pColaNew=queue_create();
    pColaReady=queue_create();
    pColaExit=queue_create();
    pColaBlock=queue_create();
    pColaExec=queue_create();
    pColaCpu=queue_create();
    //test
    //trate de hacer el ejercicio de round robin explicado en clase
    //en el caso de los io lo puse como que volvia a retomar el proceso un tiempo dsp segun el de ej de clase

    int vn[] =  {0,2,1,3};//id proceso
    int vin[] =  {0,1,10,11};//tiempo de llegada
    int vra[] =  {9,2,5,1};//duracion
    int vra2[] =  {2,1,3,0}; //rafa 2
    int vio[] =  {2,4,1,0}; //i/o time

    for (i=0;i<=3;i++){
    	queue_push(pColaPro,CreatePCB(vn[i],vin[i],vra[i],vio[i],vra2[i]));
    }
    for (i=1;i<=1;i++){
    	queue_push(pColaCpu,CreateCPU(i,1));
    }
    q = 4;
    qt = q;

    //fin test

    tact =0;

    //Comienza algoritmo round robin
    //para que funcione los tiempo de llegada tienen q estar ordenados en la cola

    while (!queue_is_empty(pColaPro) &&  (ptmp=queue_peek(pColaPro))->in <= tact ){

    				ptmp = queue_pop(pColaPro);
    				printf("Saco  proceso %d tiempo cpu %d\n",ptmp->pid,tact);
    				queue_push(pColaNew,ptmp);
    		}

    while (!queue_is_empty(pColaPro) || !queue_is_empty(pColaNew) || !queue_is_empty(pColaReady) || !queue_is_empty(pColaBlock)  || !queue_is_empty(pColaExec)  )
    {//mientras haya procesos en alguna cola

    	if (qt == 0) qt = q;




    	while (!queue_is_empty(pColaPro) &&  (ptmp=queue_peek(pColaPro))->in <= tact ){

				ptmp = queue_pop(pColaPro);
				printf("Saco  proceso %d tiempo cpu %d\n",ptmp->pid,tact);
				queue_push(pColaNew,ptmp);
		}


    	if (!queue_is_empty(pColaNew) ){

				ptmp = queue_pop(pColaNew);
				printf("Ready  proceso %d tiempo cpu %d\n",ptmp->pid,tact);
				queue_push(pColaReady,ptmp);
		}
    	//fin prioridad los que acaban de entrar


		while (!queue_is_empty(pColaReady) && !queue_is_empty(pColaCpu) ){
			ptmp = queue_pop(pColaReady);
			pcpu = queue_pop(pColaCpu);
			ptmp->cpuexec=pcpu;
			printf("Exec  proceso %d tiempo cpu %d\n",ptmp->pid,tact);
			queue_push(pColaExec,ptmp);
		}


    	if (!queue_is_empty(pColaExec) ){

    			qt = qt - 1;
				ptmp = queue_pop(pColaExec);
				if ( ptmp->rafaga > 0)
				{
					ptmp->rafaga = ptmp->rafaga - 1;
				}

				if ( qt == 0)
				{
						if ( ptmp->rafaga == 0)
							{
								if ( ptmp->io > 0){
									ptmp->io++; // le sumo uno porque abajo se lo saco y se lo tengo q sacar en el prox ciclo
									ptmp->rafaga = ptmp->r2;
									ptmp->r2 = 0;
									printf("Block proceso %d tiempo cpu %d\n",ptmp->pid,tact);
									queue_push(pColaCpu,ptmp->cpuexec);
									queue_push(pColaBlock,ptmp);
								}
								else{
									ptmp->remaing = tact;
									printf("Termina   proceso %d tiempo cpu %d\n",ptmp->pid,tact);
									queue_push(pColaCpu,ptmp->cpuexec);
									queue_push(pColaExit,ptmp);
								}
							}
							else {

							printf("%d libero x q proceso %d tiempo cpu %d\n",qt,ptmp->pid,tact);
							queue_push(pColaCpu,ptmp->cpuexec);
							queue_push(pColaReady,ptmp);
							}


				}


				if (qt > 0 ) {
					if ( ptmp->rafaga == 0)
					{
						qt=q;
						if ( ptmp->io > 0){
							ptmp->io++; // le sumo uno porque abajo se lo saco y se lo tengo q sacar en el prox ciclo
							ptmp->rafaga = ptmp->r2;
							ptmp->r2 = 0;
							printf("Block proceso %d tiempo cpu %d\n",ptmp->pid,tact);
							queue_push(pColaCpu,ptmp->cpuexec);
							queue_push(pColaBlock,ptmp);
						}
						else{
							ptmp->remaing = tact;
							printf("Termina   proceso %d tiempo cpu %d\n",ptmp->pid,tact);
							queue_push(pColaCpu,ptmp->cpuexec);
							queue_push(pColaExit,ptmp);
						}
					}
					else {
						//printf("%d libero x q proceso %d tiempo cpu %d\n",qt,ptmp->pid,tact);
						queue_push(pColaExec,ptmp);
					}
				}
		}

		if (!queue_is_empty(pColaBlock)  ){
			ptmp = queue_pop(pColaBlock);
			ptmp->io=ptmp->io-1;
			if ( ptmp->io > 0){
				queue_push(pColaBlock,ptmp);
			}
			else {
				if (  ptmp->rafaga >0)
				{
					printf("readyblock proceso %d tiempo cpu %d\n",ptmp->pid,tact);
					queue_push(pColaReady,ptmp);
				}else{
					ptmp->remaing = tact;
					printf("Termina   proceso %d tiempo cpu %d\n",ptmp->pid,tact);
					queue_push(pColaExit,ptmp);
				}
			}
		}

    	tact++;

}





   	while (!queue_is_empty(pColaExit)){
				ptmp = queue_pop(pColaExit);

				free(ptmp);
		}
    ///free(ptmp); acordarse de eliminar los punteros
    return(0);
};



struct CPU *CreateCPU (int id,int speed) {
    struct CPU *proc;
    proc = malloc(sizeof(struct CPU));
    if (proc == NULL) {
        printf("ERROR");
        exit(1);
    };
    proc->id = id;
    proc->speed = speed;
    return(proc);
};

struct PCB *CreatePCB (int pid,int in,int raf,int io, int raf2) {
    struct PCB *proc;
    proc = malloc(sizeof(struct PCB));
    if (proc == NULL) {
        printf("ERROR");
        exit(1);
    };
    proc->pid = pid;
    proc->rafaga = raf;
    proc->io = io;
    proc->r2 = raf2;
    proc->in = in;
    return(proc);
};


