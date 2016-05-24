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
int cargar_archivo_configuracion() {
	//TODO cargar archivo de configuracion y a partir de eso setear los valores
	TAMANIO_FRAME = 50;
	CANTIDAD_FRAMES = 2000;
	return 0;
}

void crear_memoria_principal() {
	char* datos = malloc(TAMANIO_MEMORIA_PRINCIPAL);
	memset(datos, '\0', TAMANIO_MEMORIA_PRINCIPAL);
	MEMORIA_PRINCIPAL = datos;
}

//solo para test
void inicializacion_para_test(int tamanio_frame, int cantidad_frame) {
	CANTIDAD_FRAMES = cantidad_frame;
	TAMANIO_FRAME = tamanio_frame;
}

void liberar_memoria_principal() {
	free(MEMORIA_PRINCIPAL);
}

int cargar_nuevo_programa(int pid, int paginas_requeridas_del_proceso, char * codigo_programa) {

	int pudo_cargar_swap = cargar_nuevo_programa_en_swap(pid, paginas_requeridas_del_proceso, codigo_programa);
	if (pudo_cargar_swap != -1 )
	{
		t_tabla_de_paginas * tabla = crear_tabla_de_pagina_de_un_proceso(pid, paginas_requeridas_del_proceso);

		/* ESTO SE SUPONE QUE NO ES NECESARIO YA QUE NO CARGO NADA EN UMC
		int pagina=0;
		for( 0 ; pagina < tabla->paginas_totales; pagina ++){
			int frame_libre = buscar_frame_libre();
			asignar_frame_a_una_pagina(tabla, frame_libre, pagina);
		}*/
		return 0; // se pudo cargar el programa correctamente
	}
	else
	{
		return -1;//no se pudo cargar el programa en swap
	}

}


int buscar_frame_libre(){
	//todo devolver -1 si no hay libres
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
	// Cuando la paginna no tiene asignado frame es -1
	return tabla->entradas[pagina].frame;

}

char* leer_frame_de_memoria_principal(int frame, int offset, int size) {

	char* datos = malloc(size);
	memcpy(datos, MEMORIA_PRINCIPAL + (frame * TAMANIO_FRAME) + offset, size);
	return datos;
}

void escribir_frame_de_memoria_principal(int frame, int offset, int size, char* datos) {
	//TODO esta funcion solo debe ser llamada si datos =< tamanio frame.
	//TODO la funciona que la llama deberia cortar los datos en un array de datos de tamanio de frame

	//TODO hacer test/analizar que pasa si los datos son menores a un frame(para que no queden bytes feos

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
	// 2) si no esta, busco el frame de la pagina en la tabla (UMC)
	// 3) si no esta, lo voy a buscar a swap y actualizo lo que sea necesario
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
			//pedir_a_swap_la_pagina_y_actualizar_memoria_principal(tabla->pid, pagina, frame_de_pagina);
			return frame_de_pagina;
		}

	}

}

void pedir_a_swap_la_pagina_y_actualizar_memoria_principal(int pid, int pagina, int frame_de_pagina){
	char * datos =  leer_pagina_de_swap(pid, pagina);
	escribir_pagina_de_programa(pid, pagina, 0, TAMANIO_FRAME, datos);
}


int cargar_nuevo_programa_en_swap(int pid, int paginas_requeridas_del_proceso, char *codigo_programa){
	return 0;
}

char * leer_pagina_de_swap(int pid, int pagina){
	return "";
}

int escribir_pagina_de_swap(int pid, int pagina, char * datos){
	return -1;
}

int finalizar_programa_de_swap(int pid){
	return -1;
}

int buscar_en_tlb_frame_de_pagina(int pid, int pagina){

}

int darle_frame_a_una_pagina(t_tabla_de_paginas* tabla, int pagina){
	// 1) si tengo cupo para pedir, pido y asigno
	//    1.1) busco frame libre
	//		  1.1.1) si hay libre en la memoria lo asigno
	// 		  1.1.2) si no hay lugar libre en la memoria hago un reemplazo

	// 2) reemplazar
	//TODO REVISAR BIEN


	//para los test deberia ser lo sufiecientemente grande para que no se produzcan reemplazos


	if(tiene_tabla_mas_paginas_para_pedir(tabla))
	{
		int frame = buscar_frame_libre();
		if(frame !=-1)
		{
			asignar_frame_a_una_pagina(tabla, frame, pagina);
			return frame;
		}
		else
		{

			int frame_conseguido= seleccionar_frame_victima(tabla);
			// ALGORITMO DE REEAMPLZADO
			/*
			if(tiene_algun_frame(tabla))
			{
			int frame_conseguido= reemplazar_frame(tabla);
			//marca_no_valida_entrada(tabla,frame_conseguido);
			actualizar_reemplazo(tabla, frame_conseguido, pagina);
			return frame_conseguido;
			}
			*/
		}
	}
	else
	{
		int frame_conseguido= seleccionar_frame_victima(tabla);
	}
}

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

int seleccionar_frame_victima(t_tabla_de_paginas* tabla)
{
	//ACA SELECCIONO EL FRAME CON CONTENIDO SEGUN ALGORITMO Y LO GUARDO EN SWAP. LUEGO DEVUELVO EL
	//FRAME DE LA VICTIMA PARA QUE SEA UTILIZADO POR OTRA PAGINA
	int frame_victima;
	switch(ALGORITMO_REEMPLAZO){
	case 1: //clock
		//frame= reemplazar_clock_(tabla);
		break;

	case 2: //clock M
		//frame= reemplazar_clock_modificado(tabla);
		break;
	case 99: //algoritmo_test
			//frame= reemplazar_test(tabla);
			break;
	}

	return frame_victima;
}

void actualizar_reemplazo(t_tabla_de_paginas* tabla, int frame_a_asignar,int pagina){
	tabla->entradas[pagina].frame=frame_a_asignar;
	tabla->entradas[pagina].utilizado=1;
	tabla->frames_en_uso +=1;


	//todo eze: aca tengo qeu buscar el frame que corresponda y reiniciar los
	// los valores de uso y modificacion
}





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

	//A PARTIR DE LA RESPUESTA DE ESCRITURA LE RESPONDO A CPU SI PUDE O NO ESCRIBIR
}

char* leer_pagina_de_programa(int pid, int pagina, int offset, int size){

		t_tabla_de_paginas * tabla = buscar_tabla_de_paginas_de_pid(pid);
		int frame = buscar_frame_de_una_pagina(tabla, pagina);

		if(frame != -1)
		{
			//actualizar_frame(frame, tabla); // segun el algoritmo
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
