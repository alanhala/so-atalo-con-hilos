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

int inicializar_estructuras() {
	int result = cargar_configuracion();
	inicializar_semaforos();
	TAMANIO_MEMORIA_PRINCIPAL = TAMANIO_FRAME * CANTIDAD_FRAMES;
	crear_memoria_principal();
	lista_tabla_de_paginas = list_create();
	crear_lista_frames();
	return 0;
}

void inicializar_semaforos() {
	sem_init(&mut_tabla_de_paginas, 0, 1);
	sem_init(&mut_lista_frames, 0, 1);

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

void finalizar_programa(int pid){
	//TODO aca no se le debe enviar nada a swap, debe hacerse en el switch del protocolo
	// yo aca solo manejo las estructuras de UMC

	t_tabla_de_paginas* tabla = buscar_tabla_de_paginas_de_pid(pid);

	int pagina=0;
	for(0; pagina < tabla->paginas_totales ; pagina++ ){
		int frame = devolver_frame_de_pagina(tabla, pagina);
		if (frame >=0)// por si nunca se asigno
			marcar_frame_como_libre(frame);
	}

	int pid_iguales(t_tabla_de_paginas *tabla) {
			return (tabla->pid == pid);
		}

	list_remove_by_condition(lista_tabla_de_paginas, pid_iguales);


}

t_entrada_tabla_de_paginas* inicializar_paginas(int paginas_requeridas_del_proceso) {
	t_entrada_tabla_de_paginas* entradas = malloc(sizeof(t_entrada_tabla_de_paginas)* paginas_requeridas_del_proceso);
	int i=0;
	while(i<paginas_requeridas_del_proceso)
	{
		entradas[i].frame=-1; // para que cuando busque el frame y no tenga devuelva eso
		entradas[i].modificado=0;
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
	return datos;
}

void escribir_frame_de_memoria_principal(int frame, int offset, int size, char* datos) {
	memcpy(MEMORIA_PRINCIPAL + (frame * TAMANIO_FRAME) + offset, datos, size);
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

	if (SWAP_MOCK_ENABLE)
		return cargar_nuevo_programa_en_swap_mock(pid, paginas_requeridas_del_proceso, codigo_programa);

		t_iniciar_programa_en_swap *carga = malloc(sizeof(t_iniciar_programa_en_swap));
		memset(carga,0,sizeof(t_iniciar_programa_en_swap));

		carga->pid = pid;
		carga->paginas_necesarias = paginas_requeridas_del_proceso;
		carga->codigo_programa= codigo_programa;

		t_stream *buffer = serializar_mensaje(1,carga);

		int bytes_enviados = send(SWAP_SOCKET_DESCRIPTOR, buffer, buffer->size, 0);

		t_header *aHeader = malloc(sizeof(t_header));

		char 	buffer_header[5];	//Buffer donde se almacena el header recibido

		int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
				bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

		bytes_recibidos_header = recv(SWAP_SOCKET_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

		char buffer_recv[buffer_header[1]]; 	//El buffer para recibir el mensaje se crea con la longitud recibida

		bytes_recibidos = recv(SWAP_SOCKET_DESCRIPTOR,buffer_recv,buffer_header[1],0);

		t_respuesta_iniciar_programa_en_swap * respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_swap));

		respuesta = (t_respuesta_iniciar_programa_en_swap*)deserealizar_mensaje(buffer_header[0], buffer_recv);

	return respuesta;
}

char * leer_pagina_de_swap(int pid, int pagina){

	if(SWAP_MOCK_ENABLE)
		return leer_pagina_de_swap_mock(pid, pagina);

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

	return respuesta; //debe devolver esto si no leyo bien "~/-1"
}

int escribir_pagina_de_swap(int pid, int pagina, char * datos){

	if (SWAP_MOCK_ENABLE)
		return escribir_pagina_de_swap_mock(pid, pagina, datos);

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

	return respuesta;
}

int finalizar_programa_de_swap(int pid){

	if(SWAP_MOCK_ENABLE)
		return finalizar_programa_de_swap_mock(pid);
	return -1;
}

int buscar_en_tlb_frame_de_pagina(int pid, int pagina){

}

int conseguir_frame_mediante_reemplazo(t_tabla_de_paginas* tabla, int pagina) {
	int pagina_victima = seleccionar_pagina_victima(tabla) ;
	int frame_victima = (tabla->entradas[pagina_victima]).frame ;
	printf("\n frame victima %d\n", frame_victima);
	char* contenido_frame_victima = leer_frame_de_memoria_principal(frame_victima, 0, TAMANIO_FRAME);
	printf("\n contenido de frame victima a escribir en swap :  %s en pagina victima %d\n", contenido_frame_victima, pagina_victima);
	//int pagina_victima = buscar_pagina_de_frame_en_tabla_de_paginas(tabla, frame_victima);
	// TODO IF PAGINA_VICTIMA FUE MODIFICADO, SINO ES AL PEDO
	escribir_pagina_de_swap(tabla->pid, pagina_victima,	contenido_frame_victima);
	//escribir_pagina_de_swap(tabla->pid, pagina,	contenido_frame_victima);
	//frame = frame_victima;
	char* contenido_pagina_a_actualizar = leer_pagina_de_swap(tabla->pid, pagina);
	//char* contenido_pagina_a_actualizar = leer_pagina_de_swap(tabla->pid, pagina_victima);
	printf("\n contenido a actualizar :  %s en pagina victima %d\n", contenido_pagina_a_actualizar, pagina);
	escribir_frame_de_memoria_principal(frame_victima, 0, TAMANIO_FRAME, contenido_pagina_a_actualizar);
	actualizar_reemplazo(tabla, frame_victima, pagina, pagina_victima);
	return frame_victima;

	/*int frame_victima = seleccionar_frame_victima(tabla);
	//int frame_victima= (tabla->entradas[pagina_victima]).frame;
	char* contenido_frame_victima = leer_frame_de_memoria_principal(frame_victima, 0, TAMANIO_FRAME);
	int pagina_victima = buscar_pagina_de_frame_en_tabla_de_paginas(tabla, frame_victima);
	// TODO IF PAGINA_VICTIMA FUE MODIFICADO, SINO ES AL PEDO
	escribir_pagina_de_swap(tabla->pid, pagina_victima,	contenido_frame_victima);
	//escribir_pagina_de_swap(tabla->pid, pagina,	contenido_frame_victima);
	frame = frame_victima;
	char* contenido_pagina_a_actualizar = leer_pagina_de_swap(tabla->pid, pagina);
	//char* contenido_pagina_a_actualizar = leer_pagina_de_swap(tabla->pid, pagina_victima);

	escribir_frame_de_memoria_principal(frame, 0, TAMANIO_FRAME, contenido_pagina_a_actualizar);
	actualizar_reemplazo(tabla, frame, pagina, pagina_victima);
	return frame; */

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
	for (i; i < tabla->paginas_totales; i++){ //TODO testear si es menor o menor igual (creo que menor)
		if ((tabla->entradas[i]).frame == frame_buscado && frame_buscado != -1){
			//TODO IMPORTANTE && tabla->entradas[i]->asignado == 1 QUE ESTA ASIGNADO Y NO ES EL DRAFT QUE QUEDO
			pagina = i;

		}

	}
	return pagina;
}

//obsoleto
void actualizar_frame(t_tabla_de_paginas * tabla, int frame){
	switch(ALGORITMO_REEMPLAZO){


	case 1: //algoritmo clock
		break;
	case 2: //algoritmo clock modificado
		break;
	case 99: //algoritmo test
			break;
	}


}
int reemplazar_test(t_tabla_de_paginas * tabla);
int seleccionar_pagina_victima(t_tabla_de_paginas* tabla)
{
	//ACA SELECCIONO EL FRAME CON CONTENIDO SEGUN ALGORITMO Y LO GUARDO EN SWAP. LUEGO DEVUELVO EL
	//FRAME DE LA VICTIMA PARA QUE SEA UTILIZADO POR OTRA PAGINA
	int pagina_victima ;
	switch(ALGORITMO_REEMPLAZO){
	case 1: //clock
		//frame= reemplazar_clock_(tabla);
		break;

	case 2: //clock M
		//frame= reemplazar_clock_modificado(tabla);
		break;
	case 99: //algoritmo_test
		pagina_victima= reemplazar_test(tabla);
		break;
	}

	return pagina_victima;
}

int reemplazar_test(t_tabla_de_paginas * tabla){

	int pagina=-1;
	int i = 0;
	for(i; i<tabla->paginas_totales; i++){

		if((tabla->entradas[i]).frame != -1){
			pagina = i;
			break;
		}
	}
	return pagina;
}

void actualizar_reemplazo(t_tabla_de_paginas* tabla, int frame_a_asignar,int pagina, int pagina_victima){
	tabla->entradas[pagina].frame=frame_a_asignar;
	//tabla->entradas[pagina].utilizado=1;
	//tabla->frames_en_uso +=1;


	tabla->entradas[pagina_victima].frame = -1;

	//todo eze: analizar
}




//obsoleto
void marca_no_utilizada_entrada(t_tabla_de_paginas* tabla, int frame)
{
	int pagina = dame_pagina_de_un_frame_para_tabla(tabla, frame);
	tabla->entradas[pagina].utilizado=0;
}

int dame_pagina_de_un_frame_para_tabla(t_tabla_de_paginas *tabla, int frame){
	t_entrada_tabla_de_paginas* entradas = tabla->entradas;

	//TODO eze: testear si con i < size o <=
	int i=0;
	for(i ;i< list_size(entradas); i++ )
	{
		if(entradas[i].frame == frame && entradas[i].utilizado)
		{
		   return i;
		}

	}
	return -1;
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
		//ver el tema de los modificados
		//actualizar_frame(frame, tabla); //aca varia segun el algoritmo de reemplazo
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
			return leer_frame_de_memoria_principal(frame, offset, size);
		}
		else
		{
			return "~/-1"; //no pude leer memoria
		}
}

// TLB

tabla_tlb* crear_tlb(){
	tabla_tlb* tabla = malloc(sizeof(tabla_tlb));
	entrada_tlb* entradas= malloc(sizeof(entrada_tlb)*CANTIDAD_ENTRADAS_TLB);
	tabla->entradas = entradas;
	return tabla;
}




// CONFIGURACION

void set_cantidad_entradas_tlb(int entradas){
	CANTIDAD_ENTRADAS_TLB =entradas;
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


	if (!strcmp(algoritmo, "CLOCK") || !strcmp(algoritmo, "clock"))
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
