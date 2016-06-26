/*
 * memoriaPrincipal.c
 *
 *  Created on: 6/5/2016
 *      Author: utnso
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "socket.h"
#include <commons/collections/list.h>
#include <semaphore.h>
#include "memoriaPrincipal.h"
#include "protocoloUMC.h"
#include "main.h"

/*
Para usar LOGS
extern t_log *trace_log_UMC
log_trace(trace_log_UMC,"<lo_que_quieran_loggear>");
*/


int inicializar_estructuras() {
	inicializar_semaforos();
	lista_cpu_context = list_create();
	TAMANIO_MEMORIA_PRINCIPAL = TAMANIO_FRAME * CANTIDAD_FRAMES;
	crear_memoria_principal();
	TLB = crear_tlb();
	lista_tabla_de_paginas = list_create();
	crear_lista_frames();
	set_test();
	CANTIDAD_MAXIMA_PROGRAMAS = CANTIDAD_FRAMES / MAX_FRAMES_POR_PROCESO;
	crear_swap_mock();


	return 0;
}

void inicializar_semaforos() {
	sem_init(&mut_tabla_de_paginas, 0, 1);
	sem_init(&mut_lista_frames, 0, 1);

	sem_init(&mut_lista_cpu_context, 0, 1);
	sem_init(&mut_memoria_principal, 0, 1);
	sem_init(&mut_swap, 0, 1);
	sem_init(&mut_tlb, 0, 1);


}

void crear_memoria_principal() {
	char* datos = malloc(TAMANIO_MEMORIA_PRINCIPAL);
	memset(datos, '\0', TAMANIO_MEMORIA_PRINCIPAL);
	MEMORIA_PRINCIPAL = datos;
}

void liberar_memoria_principal() {
	free(MEMORIA_PRINCIPAL);
}

int cargar_nuevo_programa(int pid, int paginas_requeridas_del_proceso, char * codigo_programa) {
	if (lista_tabla_de_paginas->elements_count == CANTIDAD_MAXIMA_PROGRAMAS){
		printf("Se supero la cantidad maximas de procesos activos");
		return -1;
	}

	int pudo_cargar_swap = cargar_nuevo_programa_en_swap(pid, paginas_requeridas_del_proceso, codigo_programa);
	if (pudo_cargar_swap != -1 )
	{
		t_tabla_de_paginas * tabla = crear_tabla_de_pagina_de_un_proceso(pid, paginas_requeridas_del_proceso);

		return 0; // se pudo cargar el programa correctamente
	}
	else
	{
		return -1;//no se pudo cargar el programa en swap
	}

}


int buscar_frame_libre(){
	//TODO IMPORTANTE devolver -1 si no hay libres
	int frame_libre(t_frame *frame) {
			return (frame->asignado == 0);
		}

	t_frame* frame_encontrado = list_find(lista_frames,	(void*) frame_libre);

	return frame_encontrado->frame;
}

int finalizar_programa(int pid){
	//TODO aca no se le debe enviar nada a swap, debe hacerse en el switch del protocolo
	// yo aca solo manejo las estructuras de UMC

	t_tabla_de_paginas* tabla = buscar_tabla_de_paginas_de_pid(pid);
	flush_tlb(pid);
	int pagina=0;
	for(0; pagina < tabla->paginas_totales ; pagina++ ){
		int frame = devolver_frame_de_pagina(tabla, pagina);
		if (frame >=0)// por si nunca se asigno
			marcar_frame_como_libre(frame);
	}

	int pid_iguales(t_tabla_de_paginas *tabla) {
			return (tabla->pid == pid);
		}

	t_tabla_de_paginas * aux = list_remove_by_condition(lista_tabla_de_paginas, pid_iguales);
	free(aux);
	return 0;
}

t_entrada_tabla_de_paginas* inicializar_paginas(int paginas_requeridas_del_proceso) {
	t_entrada_tabla_de_paginas* entradas = malloc(sizeof(t_entrada_tabla_de_paginas)* paginas_requeridas_del_proceso);
	int i=0;
	while(i<paginas_requeridas_del_proceso)
	{
		entradas[i].frame=-1; // para que cuando busque el frame y no tenga devuelva eso
		entradas[i].segunda_oportunidad =1;
		entradas[i].modificado=0;
		entradas[i].lru =0;
		i++;
		//TODO INICIARLIZAR TODO LO QUE HAGA FALTA
	}
	return entradas;
}

t_tabla_de_paginas * crear_tabla_de_pagina_de_un_proceso(int pid, int paginas_requeridas_del_proceso) {

	t_entrada_tabla_de_paginas* entradas = inicializar_paginas(paginas_requeridas_del_proceso);
	t_tabla_de_paginas* nueva_tabla = malloc(sizeof(t_tabla_de_paginas));
	nueva_tabla->pid = pid;
	nueva_tabla->paginas_totales = paginas_requeridas_del_proceso;
	nueva_tabla->entradas = entradas;
	nueva_tabla->frames_en_uso=0;
	nueva_tabla->indice_segunda_oportunidad=0;
	sem_wait(&mut_tabla_de_paginas);
	list_add(lista_tabla_de_paginas, nueva_tabla);
	sem_post(&mut_tabla_de_paginas);

	return nueva_tabla;
}

void asignar_frame_a_una_pagina(t_tabla_de_paginas* tabla, int frame_a_asignar,	int pagina) {
	tabla->entradas[pagina].frame = frame_a_asignar;
	tabla->frames_en_uso ++;
	int frames_iguales(t_frame *frame) {
				return (frame->frame == frame_a_asignar);
			}

	t_frame * frame_a_modificar = list_find(lista_frames, (void *)frames_iguales);
	frame_a_modificar->asignado = 1;

}

int devolver_frame_de_pagina(t_tabla_de_paginas* tabla, int pagina) {
	// Cuando la pagina no tiene asignado frame es -1
	return tabla->entradas[pagina].frame;

}

char* leer_frame_de_memoria_principal(int frame, int offset, int size) {
	char* datos = malloc(size);
	memcpy(datos, MEMORIA_PRINCIPAL + (frame * TAMANIO_FRAME) + offset, size);
	usleep(RETARDO*1000);
	return datos;
}

void escribir_frame_de_memoria_principal(int frame, int offset, int size, char* datos) {
	memcpy(MEMORIA_PRINCIPAL + (frame * TAMANIO_FRAME) + offset, datos, size);
	usleep(RETARDO*1000);
}

t_tabla_de_paginas* buscar_tabla_de_paginas_de_pid(int pid_buscado) {
	sem_wait(&mut_tabla_de_paginas);

	int pid_iguales(t_tabla_de_paginas *tabla) {
		return (tabla->pid == pid_buscado);
	}

	t_tabla_de_paginas* tabla = list_find(lista_tabla_de_paginas,
			(void*) pid_iguales);
	sem_post(&mut_tabla_de_paginas);
	return tabla;

}

void crear_lista_frames() {
	sem_wait(&mut_lista_frames);
	lista_frames = list_create();
	int i = 0;
	while (i < CANTIDAD_FRAMES) {
		agregar_frame_a_lista_de_frames(i);
		i++;
	}
	sem_post(&mut_lista_frames);
}

void agregar_frame_a_lista_de_frames(int numero_de_frame) {
	t_frame* nuevo_frame = malloc(sizeof(t_frame));
	nuevo_frame->frame = numero_de_frame;
	nuevo_frame->asignado = 0;
	list_add(lista_frames, nuevo_frame);
}

void marcar_frame_como_libre(int numero_de_frame){
	int frames_iguales(t_frame *frame) {
					return (frame->frame == numero_de_frame);
				}
	t_frame * frame_a_modificar = list_find(lista_frames, (void *)frames_iguales);
	frame_a_modificar->asignado = 0;

}

int buscar_frame_de_una_pagina(t_tabla_de_paginas* tabla, int pagina){

	// 1) buscar en tlb
	// 2) si no esta en tlb, busco el frame de la pagina en la tabla (UMC)
	// 3) si no esta en umc, selecciono un frame victica, guardo la pagina correspondiente en swap
	// y voy a buscar el valor de la pagina que quiero a swap. actualizo los valores que correspondan
	// ver el tema de los limites
	int frame_de_pagina = -1;
	if(TLB_HABILITADA)
	{
		puts("Busco frame en la TLB"); //TODO Modificar el print
		frame_de_pagina = buscar_en_tlb_frame_de_pagina(tabla->pid, pagina);
	}

	if(frame_de_pagina == -1)
	{
		frame_de_pagina = devolver_frame_de_pagina(tabla, pagina);
		if(frame_de_pagina == -1 )
		{
			frame_de_pagina = darle_frame_a_una_pagina(tabla, pagina);
			char * contenido =leer_pagina_de_swap(tabla->pid, pagina);
			escribir_frame_de_memoria_principal(frame_de_pagina, 0, TAMANIO_FRAME, contenido);
			return frame_de_pagina;
		}

	}

	return frame_de_pagina;

}

int cargar_nuevo_programa_en_swap(int pid, int paginas_requeridas_del_proceso, char *codigo_programa){
	sem_wait(&mut_swap);
	t_iniciar_programa_en_swap *carga = malloc(sizeof(t_iniciar_programa_en_swap));
	memset(carga,0,sizeof(t_iniciar_programa_en_swap));

	carga->pid = pid;
	carga->paginas_necesarias = paginas_requeridas_del_proceso;
	carga->codigo_programa= codigo_programa;

	t_stream *buffer = serializar_mensaje(1,carga);

	int bytes_enviados = send(SWAP_SOCKET_DESCRIPTOR, buffer->datos, buffer->size, 0);

	t_header *aHeader = malloc(sizeof(t_header));

	char 	buffer_header[5];	//Buffer donde se almacena el header recibido

	int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
			bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

	bytes_recibidos_header = recv(SWAP_SOCKET_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

	char buffer_recv[buffer_header[1]]; 	//El buffer para recibir el mensaje se crea con la longitud recibida

	bytes_recibidos = recv(SWAP_SOCKET_DESCRIPTOR,buffer_recv,buffer_header[1],0);

	t_respuesta_iniciar_programa_en_swap * respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_swap));

	respuesta = (t_respuesta_iniciar_programa_en_swap*)deserealizar_mensaje(buffer_header[0], buffer_recv);
	sem_post(&mut_swap);
	if (SWAP_MOCK_ENABLE)
			return cargar_nuevo_programa_en_swap_mock(pid, paginas_requeridas_del_proceso, codigo_programa);


	return respuesta->cargado_correctamente;
}

char * leer_pagina_de_swap(int pid, int pagina){

	sem_wait(&mut_swap);
	t_leer_pagina_swap *lectura = malloc(sizeof(t_leer_pagina_swap));

	memset(lectura,0,sizeof(t_leer_pagina_swap));

	lectura->pid = pid;
	lectura->pagina = pagina;

	t_stream *buffer = serializar_mensaje(3,lectura);

	int bytes_enviados = send(SWAP_SOCKET_DESCRIPTOR, buffer->datos, buffer->size, 0);

	t_header *aHeader = malloc(sizeof(t_header));

	char 	buffer_header[5];	//Buffer donde se almacena el header recibido

	int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
			bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

	bytes_recibidos_header = recv(SWAP_SOCKET_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

	char buffer_recv[buffer_header[1]]; 	//El buffer para recibir el mensaje se crea con la longitud recibida

	bytes_recibidos = recv(SWAP_SOCKET_DESCRIPTOR,buffer_recv,buffer_header[1],0);

	t_respuesta_leer_pagina_swap *respuesta = malloc(sizeof(t_respuesta_leer_pagina_swap));

	respuesta = (t_respuesta_leer_pagina_swap*)deserealizar_mensaje(buffer_header[0], buffer_recv);
	sem_post(&mut_swap);
	if(SWAP_MOCK_ENABLE)
			return leer_pagina_de_swap_mock(pid, pagina);

	return respuesta->datos; //debe devolver esto si no leyo bien "~/-1"
}

int escribir_pagina_de_swap(int pid, int pagina, char * datos){
	sem_wait(&mut_swap);
	t_escribir_pagina_swap *escritura = malloc(sizeof(t_escribir_pagina_swap));

	memset(escritura,0,sizeof(t_escribir_pagina_swap));

	escritura->pid = pid;
	escritura->pagina = pagina;
	escritura->datos = datos;

	t_stream *buffer = serializar_mensaje(5,escritura);

	int bytes_enviados = send(SWAP_SOCKET_DESCRIPTOR, buffer->datos, buffer->size, 0);

	t_header *aHeader = malloc(sizeof(t_header));

	char 	buffer_header[5];	//Buffer donde se almacena el header recibido

	int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
			bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

	bytes_recibidos_header = recv(SWAP_SOCKET_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

	char buffer_recv[buffer_header[1]]; 	//El buffer para recibir el mensaje se crea con la longitud recibida

	bytes_recibidos = recv(SWAP_SOCKET_DESCRIPTOR,buffer_recv,buffer_header[1],0);

	t_respuesta_escribir_pagina_swap *respuesta = malloc(sizeof(t_respuesta_escribir_pagina_swap));

	respuesta = (t_respuesta_escribir_pagina_swap*)deserealizar_mensaje(buffer_header[0], buffer_recv);
	sem_post(&mut_swap);
	if (SWAP_MOCK_ENABLE)
		return escribir_pagina_de_swap_mock(pid, pagina, datos);
	return respuesta->escritura_correcta;
}

int finalizar_programa_de_swap(int pid){


	sem_wait(&mut_swap);

	t_finalizar_programa_en_swap *finalizar_programa = malloc(sizeof(t_finalizar_programa_en_swap));
	memset(finalizar_programa,0,sizeof(t_finalizar_programa_en_swap));


	finalizar_programa->process_id = pid;

	t_stream *buffer = serializar_mensaje(7,finalizar_programa);

	int bytes_enviados = send(SWAP_SOCKET_DESCRIPTOR,buffer->datos,buffer->size,0);

	t_header *a_header = malloc(sizeof(t_header));

	char buffer_header[5];	//Buffer donde se almacena el header recibido

	int	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
		bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje

	bytes_recibidos_header = recv(SWAP_SOCKET_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

	char	buffer_recv[buffer_header[1]];	//El buffer para recibir el mensaje se crea con la longitud recibida

	bytes_recibidos = recv(SWAP_SOCKET_DESCRIPTOR,buffer_recv,buffer_header[1],0);

	t_respuesta_finalizar_programa_swap *respuesta = malloc(sizeof(t_respuesta_finalizar_programa_swap));

	respuesta = (t_respuesta_finalizar_programa_swap*)deserealizar_mensaje(8, buffer_recv);
	sem_post(&mut_swap);
	if(SWAP_MOCK_ENABLE)
		return finalizar_programa_de_swap_mock(pid);
	return respuesta;

}

int buscar_en_tlb_frame_de_pagina(int pid, int pagina){
	//sem_wait(&mut_tlb);
	int frame= -1;
	int i= 0;
	while(i < CANTIDAD_ENTRADAS_TLB){
		if((TLB->entradas[i]).pid == pid && (TLB->entradas[i]).pagina == pagina){
			frame = (TLB->entradas[i]).frame;
			break;
		}
		i++;
	}
	//sem_post(&mut_tlb);
	return frame;
}

int conseguir_frame_mediante_reemplazo(t_tabla_de_paginas* tabla, int pagina) {

	extern t_log *trace_log_UMC;

	int pagina_victima = seleccionar_pagina_victima(tabla) ;
	int frame_victima = (tabla->entradas[pagina_victima]).frame ;
	log_trace(trace_log_UMC,"frame victima %d\n",frame_victima);
	printf("\n frame victima %d\n", frame_victima);
	char* contenido_frame_victima = leer_frame_de_memoria_principal(frame_victima, 0, TAMANIO_FRAME);
	log_trace(trace_log_UMC,"contenido de frame victima a escribir en swap: %s de la pagina victima %d\n",contenido_frame_victima,pagina_victima);
	printf("\n contenido de frame victima a escribir en swap :  %s de la pagina victima %d\n", contenido_frame_victima, pagina_victima);
	escribir_pagina_de_swap(tabla->pid, pagina_victima,contenido_frame_victima);

	//int pagina_victima = buscar_pagina_de_frame_en_tabla_de_paginas(tabla, frame_victima);
	// TODO IF PAGINA_VICTIMA FUE MODIFICADO
	if ((tabla->entradas[pagina_victima]).modificado == 1){
		escribir_pagina_de_swap(tabla->pid, pagina_victima,	contenido_frame_victima);
		printf("Actualizacion de swap al reemplazar pagina porque estaba modificado");
	}

	char* contenido_pagina_a_actualizar = leer_pagina_de_swap(tabla->pid, pagina);

	//log_trace(trace_log_UMC,"Contenido a actualizar:  %s en pagina victima %d\n",contenido_pagina_a_actualizar,pagina);
	//printf("\n contenido a actualizar :  %s en pagina victima %d\n", contenido_pagina_a_actualizar, pagina);
	log_trace(trace_log_UMC,"Contenido a actualizar:  %s en pagina victima %d\n",contenido_pagina_a_actualizar,pagina);
	printf("\n contenido a actualizar :  %s \n", contenido_pagina_a_actualizar);
	escribir_frame_de_memoria_principal(frame_victima, 0, TAMANIO_FRAME, contenido_pagina_a_actualizar);
	actualizar_reemplazo(tabla, frame_victima, pagina, pagina_victima);
	return frame_victima;


}

int darle_frame_a_una_pagina(t_tabla_de_paginas* tabla, int pagina){

	int frame = -1;
	if(tiene_tabla_mas_paginas_para_pedir(tabla))
	{
		frame = buscar_frame_libre();
		if(frame !=-1)
		{
			asignar_frame_a_una_pagina(tabla, frame, pagina);

			return frame;
		}
		else
		{
			return  conseguir_frame_mediante_reemplazo(tabla, pagina);
		}
	}
	else
	{
		return conseguir_frame_mediante_reemplazo(tabla, pagina);
	}
}

int buscar_pagina_de_frame_en_tabla_de_paginas(t_tabla_de_paginas * tabla, int frame_buscado){

	int pagina = -1;
	int i=0;
	for (0; i < tabla->paginas_totales; i++){ //TODO testear si es menor o menor igual (creo que menor)
		if ((tabla->entradas[i]).frame == frame_buscado && frame_buscado != -1){
			//TODO IMPORTANTE && tabla->entradas[i]->asignado == 1 QUE ESTA ASIGNADO Y NO ES EL DRAFT QUE QUEDO
			pagina = i;

		}

	}
	return pagina;
}

int reemplazar_test(t_tabla_de_paginas * tabla);

int seleccionar_pagina_victima(t_tabla_de_paginas* tabla)
{
	//ACA SELECCIONO EL FRAME CON CONTENIDO SEGUN ALGORITMO Y LO GUARDO EN SWAP. LUEGO DEVUELVO EL
	//FRAME DE LA VICTIMA PARA QUE SEA UTILIZADO POR OTRA PAGINA
	int pagina_victima =-1 ;
	switch(ALGORITMO_REEMPLAZO){
	case 1: //clock
		pagina_victima= reemplazar_clock(tabla);
		break;

	case 2: //clock M
		pagina_victima= reemplazar_clock_modificado(tabla);
		break;
	case 99: //algoritmo_test
		pagina_victima= reemplazar_test(tabla);
		break;
	}
	return pagina_victima;
}

int busco_cero_cero(t_tabla_de_paginas * tabla){
	int indice = tabla->indice_segunda_oportunidad;

	while(indice < tabla->paginas_totales)
	{
		if ((tabla->entradas[indice]).segunda_oportunidad == 0 && (tabla->entradas[indice]).modificado == 0 && (tabla->entradas[indice]).frame != -1)
		{
			int victima = indice;
			if (tabla->indice_segunda_oportunidad == (tabla->paginas_totales -1))
			{
				tabla->indice_segunda_oportunidad = 0;
			}
			else
			{
				tabla->indice_segunda_oportunidad = indice + 1;
			}

			return victima;
		}

		indice ++;
	}

	indice = 0;
	while(indice < tabla->indice_segunda_oportunidad)
	{
		if ((tabla->entradas[indice]).segunda_oportunidad == 0 && (tabla->entradas[indice]).modificado == 0 && (tabla->entradas[indice]).frame != -1)
		{
			int victima = indice;
			if (tabla->indice_segunda_oportunidad == (tabla->paginas_totales -1))
			{
				tabla->indice_segunda_oportunidad = 0;
			}
			else
			{
				tabla->indice_segunda_oportunidad = indice + 1;
			}
			return victima;
		}
		indice ++;
	}

	return  -1; //no encontro a ninguno
}

int busco_cero_uno(t_tabla_de_paginas * tabla){
	int indice = tabla->indice_segunda_oportunidad;

	while(indice < tabla->paginas_totales)
	{
		if ((tabla->entradas[indice]).segunda_oportunidad == 0 && (tabla->entradas[indice]).modificado == 1 && (tabla->entradas[indice]).frame != -1)
		{
			int victima = indice;
			if (tabla->indice_segunda_oportunidad == (tabla->paginas_totales -1))
			{
				tabla->indice_segunda_oportunidad = 0;
			}
			else
			{
				tabla->indice_segunda_oportunidad = indice + 1;
			}

			return victima;
		}
		if ((tabla->entradas[indice]).frame != -1)
			(tabla->entradas[indice]).segunda_oportunidad = 0;
		indice ++;
	}

	indice = 0;
	while(indice < tabla->indice_segunda_oportunidad)
	{
		if ((tabla->entradas[indice]).segunda_oportunidad == 0 && (tabla->entradas[indice]).modificado == 1 && (tabla->entradas[indice]).frame != -1)
		{
			int victima = indice;
			if (tabla->indice_segunda_oportunidad == (tabla->paginas_totales -1))
			{
				tabla->indice_segunda_oportunidad = 0;
			}
			else
			{
				tabla->indice_segunda_oportunidad = indice + 1;
			}
			return victima;
		}
		if ((tabla->entradas[indice]).frame != -1)
					(tabla->entradas[indice]).segunda_oportunidad = 0;
		indice ++;
	}

	return -1 ; //no encontro ninguno
}

int reemplazar_clock_modificado(t_tabla_de_paginas * tabla){

	puts("Reemplazo por clock modificado"); //TODO Modificar este print

	int indice = tabla->indice_segunda_oportunidad;

	int pagina_victima = busco_cero_cero(tabla);

	if (pagina_victima != -1)
		return pagina_victima;

	pagina_victima = busco_cero_uno(tabla);

	if (pagina_victima != -1)
			return pagina_victima;

	pagina_victima = busco_cero_cero(tabla);

	if (pagina_victima != -1)
		return pagina_victima;

	pagina_victima = busco_cero_uno(tabla);

	if (pagina_victima != -1)
			return pagina_victima;

	return -1; // el algoritmo no funciono

}

int reemplazar_clock(t_tabla_de_paginas * tabla){

	puts("Reemplazo por clock\n");//TODO Modificar este

	int indice = tabla->indice_segunda_oportunidad;

	while(indice < tabla->paginas_totales)
	{
		if ((tabla->entradas[indice]).segunda_oportunidad == 0 && (tabla->entradas[indice]).frame != -1)
		{
			if (tabla->indice_segunda_oportunidad == (tabla->paginas_totales -1))
			{
				tabla->indice_segunda_oportunidad = 0;
			}
			else
			{
				tabla->indice_segunda_oportunidad = indice + 1;
			}
			return indice;
			//return (tabla->entradas[indice]).frame;
		}
		if ((tabla->entradas[indice]).frame != -1)
			(tabla->entradas[indice]).segunda_oportunidad = 0;
		indice ++;
	}

	indice = 0;
	while(indice < tabla->indice_segunda_oportunidad)
	{
		if ((tabla->entradas[indice]).segunda_oportunidad == 0 && (tabla->entradas[indice]).frame != -1)
		{

			if (tabla->indice_segunda_oportunidad == (tabla->paginas_totales -1))
			{
				tabla->indice_segunda_oportunidad = 0;
			}
			else
			{
				tabla->indice_segunda_oportunidad = indice + 1;
			}
			return indice;
			//return (tabla->entradas[indice]).frame;
		}
		if ((tabla->entradas[indice]).frame != -1)
			(tabla->entradas[indice]).segunda_oportunidad = 0;
		indice ++;
	}


	while(indice < tabla->paginas_totales)
		{
			if ((tabla->entradas[indice]).segunda_oportunidad == 0 && (tabla->entradas[indice]).frame != -1)
			{

				tabla->indice_segunda_oportunidad = indice;
				return indice;
				///return (tabla->entradas[indice]).frame;
			}
			if ((tabla->entradas[indice]).frame != -1)
				(tabla->entradas[indice]).segunda_oportunidad = 0;
			indice ++;
		}

		indice = 0;
		while(indice < tabla->indice_segunda_oportunidad)
		{
			if ((tabla->entradas[indice]).segunda_oportunidad == 0 && (tabla->entradas[indice]).frame != -1)
			{

				tabla->indice_segunda_oportunidad = indice;
				return indice;
				//return (tabla->entradas[indice]).frame;
			}
			if ((tabla->entradas[indice]).frame != -1)
				(tabla->entradas[indice]).segunda_oportunidad = 0;
			indice ++;
		}


	return -1; //el algoritmo funciono mal
}

int reemplazar_test(t_tabla_de_paginas * tabla){

	int pagina=-1;
	int i = 0;
	for(i=0; i<tabla->paginas_totales; i++){

		if((tabla->entradas[i]).frame != -1){
			pagina = i;
			break;
		}
	}
	return pagina;
}

void actualizar_reemplazo(t_tabla_de_paginas* tabla, int frame_a_asignar,int pagina, int pagina_victima){

	tabla->entradas[pagina].frame=frame_a_asignar;
	tabla->entradas[pagina].segunda_oportunidad=1;
	tabla->entradas[pagina].modificado=0;


	tabla->entradas[pagina_victima].frame = -1;
	tabla->entradas[pagina_victima].segunda_oportunidad = 1;
	tabla->entradas[pagina_victima].modificado = 0;

	//todo eze: analizar
}



int tiene_tabla_mas_paginas_para_pedir(t_tabla_de_paginas* tabla)
{
	return (tabla->frames_en_uso < MAX_FRAMES_POR_PROCESO);
}

int escribir_pagina_de_programa(int pid, int pagina, int offset, int size, char * datos){
	t_tabla_de_paginas * tabla = buscar_tabla_de_paginas_de_pid(pid);
	int frame = buscar_frame_de_una_pagina(tabla, pagina);

	if(frame != -1)
	{
		(tabla->entradas[pagina]).segunda_oportunidad = 1;
		(tabla->entradas[pagina]).modificado = 1;


		//actualizar_frame(frame, tabla); //aca varia segun el algoritmo de reemplazo
		actualizar_tlb(pid, pagina, frame);
		escribir_frame_de_memoria_principal(frame, offset, size, datos);

		return 0; //escritura ok;
	}
	else
	{
		return -1;// escritura no Ok;
	}

}

char* leer_pagina_de_programa(int pid, int pagina, int offset, int size){

		t_tabla_de_paginas * tabla = buscar_tabla_de_paginas_de_pid(pid);

		int frame = buscar_frame_de_una_pagina(tabla, pagina);

		if(frame != -1)
		{
			//(tabla->entradas[pagina]).segunda_oportunidad = 1;
			actualizar_tlb(pid, pagina, frame);
			return leer_frame_de_memoria_principal(frame, offset, size);
		}
		else
		{
			return "~/-1"; //no pude leer memoria
		}
}

int dame_pid_activo(int cpu_socket_descriptor){

		sem_wait(&mut_lista_cpu_context);
		int cpu_id_iguales(t_cpu_context *cpu_context) {
			return (cpu_context->cpu_id == cpu_socket_descriptor);
		}

		t_cpu_context* cpu = list_find(lista_cpu_context,(void*) cpu_id_iguales);
		sem_post(&mut_lista_cpu_context);
		return cpu->pid_active;
}
int cambio_contexto(int cpu_id, int pid){

	flush_tlb(dame_pid_activo(cpu_id));
	int cpu_id_iguales(t_cpu_context *cpu_context) {
		if (cpu_context->cpu_id == cpu_id)
			cpu_context->pid_active = pid;
			return (cpu_context->cpu_id == cpu_id);
	}
	sem_wait(&mut_lista_cpu_context);
	t_cpu_context* cpu = list_find(lista_cpu_context,(void*) cpu_id_iguales);
	sem_post(&mut_lista_cpu_context);
	if (cpu->pid_active != pid)
		return -1;
	return 0;
}

// TLB

int buscar_victima_tlb_lru(int pid){
	//sem_wait(&mut_tlb);
	int victima = -1;
	int indice = 0;
	int mas_viejo = -1;
	while(indice < CANTIDAD_ENTRADAS_TLB){
		//if((TLB->entradas[indice]).pid == pid &&  (TLB->entradas[indice].lru) > mas_viejo
		// la linea de arriba es si le quiero pisar solo al de mi proceso
		if((TLB->entradas[indice].lru) > mas_viejo
				&&  TLB->entradas[indice].frame != -1){
			mas_viejo = (TLB->entradas[indice].lru);
			victima = indice;
		}
		indice ++;
	}
	//sem_post(&mut_tlb);
	return victima;
}

int buscar_entrada_tlb(int pid){
	//sem_wait(&mut_tlb);
	int entrada_tlb = -1;
	int i= 0;
	while(i < CANTIDAD_ENTRADAS_TLB){
		if((TLB->entradas[i]).pid == -1){
			entrada_tlb = i;
			break;
		}
		i++;
	}
	//sem_post(&mut_tlb);
	if (entrada_tlb == -1)
		entrada_tlb = buscar_victima_tlb_lru(pid);

	return entrada_tlb;


}

void lru_sumarle_uno_a_todos(){
	//sem_wait(&mut_tlb);
	int i= 0;
	while(i < CANTIDAD_ENTRADAS_TLB){
		if((TLB->entradas[i]).pid != -1){
			(TLB->entradas[i]).lru ++;
		}
		i++;
	}
	//sem_post(&mut_tlb);
}
int esta_presente_en_tlb(int pid, int pagina){

	//TODO VER COMO USAR EL MUTEX

	//sem_wait(&mut_tlb);

	int i= 0;
	while(i < CANTIDAD_ENTRADAS_TLB){
		if((TLB->entradas[i]).pid == pid && (TLB->entradas[i]).pagina == pagina){
			return i;
		}

		i++;
	}
	return  -1;
}

void actualizar_tlb(int pid, int pagina, int frame){

	int entrada = -1;

	entrada = esta_presente_en_tlb(pid,pagina);
	if (entrada == -1)
		entrada = buscar_entrada_tlb(pid);

	//sem_wait(&mut_tlb);
	(TLB->entradas[entrada]).pid = pid;
	(TLB->entradas[entrada]).lru = -1; // lo seteo en -1 para que cuando le sume uno a todos quede en 0
	(TLB->entradas[entrada]).frame = frame;
	(TLB->entradas[entrada]).pagina = pagina;
	//sem_post(&mut_tlb);
	lru_sumarle_uno_a_todos();
}



inicializar_entradas_tlb(){
	entrada_tlb* entradas= malloc(sizeof(entrada_tlb)*CANTIDAD_ENTRADAS_TLB);

	int i=0;
	while(i<CANTIDAD_ENTRADAS_TLB)
	{
		entradas[i].pid =-1;
		entradas[i].frame=-1; // para que cuando busque el frame y no tenga devuelva eso
		entradas[i].pagina =-1;
		entradas[i].lru =0;
		i++;

	}
	return entradas;

}
tabla_tlb* crear_tlb(){

	entrada_tlb* entradas = inicializar_entradas_tlb();
	tabla_tlb* tlb = malloc(sizeof(tabla_tlb));
	tlb->entradas = entradas;
	return tlb;
}



// COMANDOS

void flush_tlb(int pid){
	if (pid == -1)
	{
		void flush(t_tabla_de_paginas *tabla)
		{

			int i= 0;
			while(i < CANTIDAD_ENTRADAS_TLB){
				if((TLB->entradas[i]).pid == tabla->pid){
					(TLB->entradas[i]).pid =-1;
					(TLB->entradas[i]).lru = 0;
					(TLB->entradas[i]).pagina = -1;
					(TLB->entradas[i]).frame = -1;
				}
				i++;
			}

		}
		//sem_wait(&mut_tlb);
		list_iterate(lista_tabla_de_paginas, (void*) flush);
		//sem_post(&mut_tlb);
	}
	else
	{
		//sem_wait(&mut_tlb);
		int i= 0;
		while(i < CANTIDAD_ENTRADAS_TLB){
			if((TLB->entradas[i]).pid == pid){
				(TLB->entradas[i]).pid =-1;
				(TLB->entradas[i]).lru =0;
				(TLB->entradas[i]).pagina = -1;
				(TLB->entradas[i]).frame = -1;
			}
			i++;
		}
		//sem_post(&mut_tlb);

	}

}


void dump_memory(int pid){
	extern t_log *trace_log_UMC;
	//Contenido de memoria:
	//Datos almacenados en la memoria de todos los procesos o de un proceso en particular.
	if (pid == -1)
	{

		void dump(t_tabla_de_paginas *tabla)
		{
			log_trace(trace_log_UMC,"Contenido en memoria de proceso %d\n",tabla->pid);
			printf("			Contenido en memoria de proceso %d\n\n", tabla->pid);
			int i =0;
			for(0; i<tabla->paginas_totales; i++)
			{
				int frame =(tabla->entradas[i]).frame ;
				if (frame != -1)
				{
					log_trace(trace_log_UMC,"Contenido de la entrada %d ubicada en el frame %d\n",i, frame );
					printf("Contenido de la entrada %d ubicada en el frame %d : \n",i, frame );
					char *  contenido = leer_frame_de_memoria_principal(frame, 0, TAMANIO_FRAME);
					int posicion = 0;
					while (contenido[posicion] != '\0') {
					  if (isprint(contenido[posicion]))
						  printf("contenido frame %d : %c    ", frame, contenido[posicion]);
					  else
						  printf("~    ");
					  posicion++;
					}
					printf("\n");
//					int poshex = 0;
//					while (contenido[poshex] != '\0') {
//					  //printf("%02x   ", (unsigned int) contenido[poshex]);
//					  printf("%x   ", contenido[poshex] & 0xff);
//					  fflush(stdout);
//					  poshex++;
//					}
//					printf("\n");
					printf("\n");

				}
			}
		}

		sem_wait(&mut_tabla_de_paginas);
		list_iterate(lista_tabla_de_paginas, (void*) dump);
		sem_post(&mut_tabla_de_paginas);
	}
	else
	{
		//TODO VER SI PONGO MUTEX ACA O NO
		t_tabla_de_paginas * tabla = buscar_tabla_de_paginas_de_pid(pid);
		log_trace(trace_log_UMC,"Contenido en memoria de proceso %d\n", tabla->pid);
		printf("Contenido en memoria de proceso %d\n", tabla->pid);
		int i =0;
		for(0; i<tabla->paginas_totales; i++)
		{
			int frame =(tabla->entradas[i]).frame ;
			if (frame != -1)
			{
				log_trace(trace_log_UMC,"Contenido en memoria de proceso %d\n", tabla->pid);
				printf("Contenido de la entrada %d ubicada en el frame %d : \n",i, frame );
				char *  contenido = leer_frame_de_memoria_principal(frame, 0, TAMANIO_FRAME);
				int posicion = 0;
				while (contenido[posicion] != '\0') {
				  if (isprint(contenido[posicion]))
					  printf("contenido frame %d : %c    ", frame, contenido[posicion]);
				  else
					  printf("~");
				  posicion++;
				}
				printf("\n");
//				int poshex = 0;
//				while (contenido[poshex] != '\0') {
//				  printf("%02x   ", (unsigned int) contenido[poshex]);
//				  i++;
//				}
//				printf("\n");
				printf("\n");
			}
		}

	}
}

void dump_structs(int pid){
	extern t_log *trace_log_UMC;
	if (pid == -1){
		void dump_table(t_tabla_de_paginas * tabla){
			log_trace(trace_log_UMC,"TABLA DE PAGINAS DEL PROCESO: %d\n", tabla->pid);
			printf("		TABLA DE PAGINAS DEL PROCESO: %d \n\n\n", tabla->pid);
			log_trace(trace_log_UMC,"Paginas totales del proceso: %d\n", tabla->paginas_totales);
			printf("Paginas totales del proceso: %d \n\n",tabla->paginas_totales);
			int i =0;
			for(0; i<tabla->paginas_totales; i++){
				//TODO SACAR EL IF EN AMBOS
				if((tabla->entradas[i]).frame != -1){
				log_trace(trace_log_UMC,"Entrada %d\n",i);
				printf("		Entrada %d\n", i);
				log_trace(trace_log_UMC,"Ubicado en el frame: %d\n",(tabla->entradas[i]).frame);
				printf("Ubicado en el frame: %d\n",(tabla->entradas[i]).frame);
				log_trace(trace_log_UMC,"Segunda Oportunidad: %d\n",(tabla->entradas[i]).segunda_oportunidad);
				printf("Segunda Oportunidad: %d\n",(tabla->entradas[i]).segunda_oportunidad);
				log_trace(trace_log_UMC,"Modificado: %d\n",(tabla->entradas[i]).modificado);
				printf("Modificado: %d\n",(tabla->entradas[i]).modificado);
//				log_trace(trace_log_UMC,"Ultimo uso: %d\n\n\n",(tabla->entradas[i]).lru);
//				printf("Ultimo uso: %d\n\n\n",(tabla->entradas[i]).lru);
				}
			}
		}
			sem_wait(&mut_tabla_de_paginas);
			list_iterate(lista_tabla_de_paginas, (void*) dump_table);
			sem_post(&mut_tabla_de_paginas);
	}
	else{
		t_tabla_de_paginas * tabla = buscar_tabla_de_paginas_de_pid(pid);
		printf("		TABLA DE PAGINAS DEL PROCESO: %d \n\n\n", pid);
		printf("Paginas totales del proceso: %d \n\n",tabla->paginas_totales);
		int i =0;
		for(0; i<tabla->paginas_totales; i++){
			if((tabla->entradas[i]).frame != -1){
			printf("		Entrada %d\n", i);
			printf("Ubicado en el frame: %d\n",(tabla->entradas[i]).frame);
			printf("Segunda Oportunidad: %d\n",(tabla->entradas[i]).segunda_oportunidad);
			printf("Modificado: %d\n",(tabla->entradas[i]).modificado);

			}
		}
	}
}

void flush_memory(int pid){
	if (pid == -1)
		{
			void flush(t_tabla_de_paginas *tabla)
			{
				int i =0;
				for(0; i<tabla->paginas_totales; i++){
					(tabla->entradas[i]).modificado = 1;
				}
			}
			sem_wait(&mut_tabla_de_paginas);
			list_iterate(lista_tabla_de_paginas, (void*) flush);
			sem_post(&mut_tabla_de_paginas);
		}
	else
	{
		//TODO VER SI PONGO MUTEX ACA O NO
		t_tabla_de_paginas * tabla = buscar_tabla_de_paginas_de_pid(pid);
		int i =0;
		for(0; i<tabla->paginas_totales; i++){
			(tabla->entradas[i]).modificado = 1;
		}
	}
}





// CONFIGURACION

void set_cantidad_entradas_tlb(int entradas){
	if (entradas>0){
		TLB_HABILITADA=1;
	}
	else {
		TLB_HABILITADA=0;
	}

	CANTIDAD_ENTRADAS_TLB = entradas;
}
void set_max_frames_por_proceso(int cantidad){
	MAX_FRAMES_POR_PROCESO = cantidad;
}
void set_cantidad_frames(int cantidad_frames){
	CANTIDAD_FRAMES = cantidad_frames;
}
void set_tamanio_frame(int tamanio_frame){
	TAMANIO_FRAME= tamanio_frame;
}
void set_retardo(int retardo){
	RETARDO = retardo;
}
void set_algoritmo_reemplazo(char * algoritmo){


	if (!strcmp(algoritmo, "CLOCK") || !strcmp(algoritmo, "clock")|| !strcmp(algoritmo, "Clock"))
			ALGORITMO_REEMPLAZO = 1;
	if (!strcmp(algoritmo, "CLOCKM") || !strcmp(algoritmo, "clockM")|| !strcmp(algoritmo, "clockm") || !strcmp(algoritmo, "ClockM"))
				ALGORITMO_REEMPLAZO = 2;
	if (!strcmp(algoritmo, "test") || !strcmp(algoritmo, "TEST"))
			ALGORITMO_REEMPLAZO = 99;
}
void set_test(){
	SWAP_MOCK_ENABLE=1;
}


void set_socket_descriptor(int fd){
	SWAP_SOCKET_DESCRIPTOR = fd;
}


void crear_swap_mock(){

	char* datos = malloc(10000000);
	memset(datos, '/0', 10000000);
	SWAP_MOCK = datos;

}

int cargar_nuevo_programa_en_swap_mock(int pid, int paginas_requeridas_del_proceso, char *codigo_programa){
	memcpy(SWAP_MOCK + (pid * MAX_FRAMES_POR_PROCESO * TAMANIO_FRAME), codigo_programa, strlen(codigo_programa));
	return 0;
}

char * leer_pagina_de_swap_mock(int pid, int pagina){
	char* datos = malloc(TAMANIO_FRAME);
	memcpy(datos, SWAP_MOCK + (pid * MAX_FRAMES_POR_PROCESO * TAMANIO_FRAME) + (pagina * TAMANIO_FRAME),  TAMANIO_FRAME);
	return datos;



}

int escribir_pagina_de_swap_mock(int pid, int pagina, char* datos) {

	memcpy(SWAP_MOCK + (pid * MAX_FRAMES_POR_PROCESO * TAMANIO_FRAME) +(pagina * TAMANIO_FRAME) , datos, TAMANIO_FRAME);
	return 0;
}

int finalizar_programa_de_swap_mock(int pid){
       return 0;
}
