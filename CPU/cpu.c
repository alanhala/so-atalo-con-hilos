/*
 * cpu.c

 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include "protocoloCPU.h"



extern t_log *trace_log_CPU;
//log_trace(trace_log_CPU,"<lo_que_quieran_loggear>");


AnSISOP_funciones functions = {
	.AnSISOP_definirVariable	= definirVariable,
	.AnSISOP_obtenerPosicionVariable = obtenerPosicionVariable,
	.AnSISOP_dereferenciar	= dereferenciar,
	.AnSISOP_asignar	= asignar,
	.AnSISOP_imprimir	= imprimir,
	.AnSISOP_imprimirTexto	= imprimirTexto,
	.AnSISOP_irAlLabel 	= irALabel,
	.AnSISOP_llamarSinRetorno = llamarSinRetorno,
	.AnSISOP_llamarConRetorno = llamarConRetorno,
	.AnSISOP_finalizar = finalizar,
	.AnSISOP_retornar = retornar,
	.AnSISOP_entradaSalida = entradaSalida,
	.AnSISOP_obtenerValorCompartida = obtenerValorCompartida,
	.AnSISOP_asignarValorCompartida = asignarValorCompartida
};

AnSISOP_kernel kernel_functions = {
	.AnSISOP_signal = do_signal,
	.AnSISOP_wait = do_wait
};

t_PCB *pcb;

uint32_t tamanio_pagina;

void do_wait(t_nombre_semaforo identificador_semaforo) {
	while(string_ends_with(identificador_semaforo, "\n") || string_ends_with(identificador_semaforo, " ") || string_ends_with(identificador_semaforo, "\t"))
			identificador_semaforo = string_substring_until(identificador_semaforo, string_length((char*)identificador_semaforo)-1);
	t_PCB_serializacion * pcb_serializado = adaptar_pcb_a_serializar(get_PCB());
	pcb_serializado->mensaje = 4;
	pcb_serializado->valor_mensaje = identificador_semaforo;
	pcb_serializado->cantidad_operaciones = 0;
	pcb_serializado->resultado_mensaje = 0;
	t_stream * stream = serializar_mensaje(121,pcb_serializado);
	send(KERNEL_DESCRIPTOR, stream->datos, stream->size, 0);

	t_header *a_header = malloc(sizeof(t_header));

	char buffer_header[5];	//Buffer donde se almacena el header recibido

	int bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
			bytes_recibidos;//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

	bytes_recibidos_header = recv(KERNEL_DESCRIPTOR, buffer_header, 5,	MSG_PEEK);

	a_header = deserializar_header(buffer_header);

	int tipo = a_header->tipo;
	int length = a_header->length;

	char buffer_recv[length]; //El buffer para recibir el mensaje se crea con la longitud recibida

	if (tipo == 121) {

		int bytes_recibidos = recv(KERNEL_DESCRIPTOR, buffer_recv,
				length, 0);

		t_PCB_serializacion *recibir_pcb = malloc(sizeof(t_PCB_serializacion));

		recibir_pcb = (t_PCB_serializacion * ) deserealizar_mensaje(121, buffer_recv);
		actualizarPCB(get_PCB(), recibir_pcb);
		if (get_PCB()->program_finished == 5) {
			pthread_exit();
		}
	}
}

void do_signal(t_nombre_semaforo identificador_semaforo) {
	while(string_ends_with(identificador_semaforo, "\n") || string_ends_with(identificador_semaforo, " ") || string_ends_with(identificador_semaforo, "\t"))
		identificador_semaforo = string_substring_until(identificador_semaforo, string_length((char*)identificador_semaforo)-1);
	t_PCB_serializacion * pcb_serializado = adaptar_pcb_a_serializar(get_PCB());
	pcb_serializado->mensaje = 5;
	pcb_serializado->valor_mensaje = identificador_semaforo;
	pcb_serializado->cantidad_operaciones = 0;
	pcb_serializado->resultado_mensaje = 0;
	t_stream * stream = serializar_mensaje(121,pcb_serializado);
	send(KERNEL_DESCRIPTOR, stream->datos, stream->size, 0);
}

t_valor_variable obtenerValorCompartida(t_nombre_compartida variable) {

	while(string_ends_with(variable, "\n") || string_ends_with(variable, " ") || string_ends_with(variable, "\t"))
		variable = string_substring_until(variable, string_length((char*)variable)-1);
	t_PCB_serializacion * pcb_serializado = adaptar_pcb_a_serializar(get_PCB());
	pcb_serializado->mensaje = 1;
	pcb_serializado->valor_mensaje = variable;
	pcb_serializado->cantidad_operaciones = 0;
	pcb_serializado->valor_de_la_variable_compartida =0;
	pcb_serializado->resultado_mensaje = 0;
	t_stream * stream = serializar_mensaje(121,pcb_serializado);
	send(KERNEL_DESCRIPTOR, stream->datos, stream->size, 0);


	t_header *a_header = malloc(sizeof(t_header));

	char buffer_header[5];	//Buffer donde se almacena el header recibido

	int bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
			bytes_recibidos;//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

	bytes_recibidos_header = recv(KERNEL_DESCRIPTOR, buffer_header, 5,	MSG_PEEK);

	a_header = deserializar_header(buffer_header);

	int tipo = a_header->tipo;
	int length = a_header->length;

	char buffer_recv[length]; //El buffer para recibir el mensaje se crea con la longitud recibida

	if (tipo == 121) {

		int bytes_recibidos = recv(KERNEL_DESCRIPTOR, buffer_recv,
				length, 0);

		t_PCB_serializacion *recibir_pcb = malloc(sizeof(t_PCB_serializacion));

		recibir_pcb = (t_PCB_serializacion * ) deserealizar_mensaje(121, buffer_recv);
		log_trace(trace_log_CPU,"Valor de la variable compartida: %d\n", recibir_pcb->resultado_mensaje);
		return recibir_pcb->resultado_mensaje;
	}
	return -1; //TODO NO SE OBTUVO RESULTADO DE LA VARIABLE COMPARTIDA


}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor) {
	log_trace(trace_log_CPU,"asignarValorCompartida");

	while(string_ends_with(variable, "\n") || string_ends_with(variable, " ") || string_ends_with(variable, "\t"))
		variable = string_substring_until(variable, string_length((char*)variable)-1);
	t_PCB_serializacion * pcb_serializado = adaptar_pcb_a_serializar(get_PCB());
	pcb_serializado->mensaje = 2;
	pcb_serializado->valor_mensaje = variable;
	pcb_serializado->cantidad_operaciones = 0;
	pcb_serializado->valor_de_la_variable_compartida =valor;
	pcb_serializado->resultado_mensaje = 0;
	t_stream * stream = serializar_mensaje(121,pcb_serializado);
	send(KERNEL_DESCRIPTOR, stream->datos, stream->size, 0);

	t_header *a_header = malloc(sizeof(t_header));

	char buffer_header[5];	//Buffer donde se almacena el header recibido

	int bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
			bytes_recibidos;//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

	bytes_recibidos_header = recv(KERNEL_DESCRIPTOR, buffer_header, 5,	MSG_PEEK);

	a_header = deserializar_header(buffer_header);

	int tipo = a_header->tipo;
	int length = a_header->length;

	char buffer_recv[length]; //El buffer para recibir el mensaje se crea con la longitud recibida

	if (tipo == 121) {

		int bytes_recibidos = recv(KERNEL_DESCRIPTOR, buffer_recv,
				length, 0);

		t_PCB_serializacion *recibir_pcb = malloc(sizeof(t_PCB_serializacion));

		recibir_pcb = (t_PCB_serializacion * ) deserealizar_mensaje(121, buffer_recv);

		//ACA VER SI RECIBO ALGO O NO
	}
	return valor;

}

void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo) {


	while(string_ends_with(dispositivo, "\n") || string_ends_with(dispositivo, " ") || string_ends_with(dispositivo, "\t"))
			dispositivo = string_substring_until(dispositivo, string_length((char*)dispositivo)-1);
	t_PCB_serializacion * pcb_serializado = adaptar_pcb_a_serializar(get_PCB());
	pcb_serializado->mensaje = 6;
	pcb_serializado->valor_mensaje = dispositivo;
	pcb_serializado->cantidad_operaciones = tiempo;
	pcb_serializado->resultado_mensaje = 0;
	t_stream * stream = serializar_mensaje(121,pcb_serializado);
	send(KERNEL_DESCRIPTOR, stream->datos, stream->size, 0);

	t_header *a_header = malloc(sizeof(t_header));

	char buffer_header[5];	//Buffer donde se almacena el header recibido

	int bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
			bytes_recibidos;//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

	bytes_recibidos_header = recv(KERNEL_DESCRIPTOR, buffer_header, 5,	MSG_PEEK);

	a_header = deserializar_header(buffer_header);

	int tipo = a_header->tipo;
	int length = a_header->length;

	char buffer_recv[length]; //El buffer para recibir el mensaje se crea con la longitud recibida

	if (tipo == 121) {

		int bytes_recibidos = recv(KERNEL_DESCRIPTOR, buffer_recv,
				length, 0);

		t_PCB_serializacion *recibir_pcb = malloc(sizeof(t_PCB_serializacion));

		recibir_pcb = (t_PCB_serializacion * ) deserealizar_mensaje(121, buffer_recv);
		actualizarPCB(get_PCB(), recibir_pcb);
		if (get_PCB()->program_finished == 6) {
			io_id = dispositivo;
			io_operations = tiempo;
			pthread_exit();
		}
	}
}

void* execute_instruction(void* instruction) {
	char* instruccion_string = (char*) instruction;
	analizadorLinea(strdup(instruccion_string), &functions, &kernel_functions);
}

void execute_next_instruction_for_process() {
	t_dato_en_memoria *instruccion = get_next_instruction();
	char *instruccion_string = ejecutar_lectura_de_dato_con_iteraciones(leer_memoria_de_umc, instruccion, tamanio_pagina);
//	int memory_used = malloc_usable_size(instruccion_string);
//	realloc(instruccion_string, memory_used+1);
//	instruccion_string[memory_used]= '\0';
	if(pcb->program_finished != 58){
		char* asd = malloc(instruccion->size + 1);
		memcpy(asd, instruccion_string, instruccion->size);
		asd[instruccion->size] = '\0';
		int program_counter = pcb->program_counter;
		pthread_t execution_thread;
		pthread_create(&execution_thread, NULL, &execute_instruction, (void*) asd);
		pthread_join(execution_thread, NULL);
		log_trace(trace_log_CPU,"Instruccion: %s", instruccion_string);
		if(program_counter == pcb->program_counter && !string_starts_with(instruccion_string, TEXT_END) && !string_starts_with(instruccion_string, TEXT_RETURN)) {
			pcb->program_counter++;
		}
	}
};


char* obtener_instruccion_de_umc(t_dato_en_memoria *instruccion) {
	return "a = a + b";
}

int cambiar_contexto(int pid){

	t_cambio_de_proceso * cambio_proceso = malloc(sizeof(t_cambio_de_proceso));
	memset(cambio_proceso,0,sizeof(t_cambio_de_proceso));

	cambio_proceso->pid = pid;

	t_stream *buffer = (t_stream*)serializar_mensaje(35,cambio_proceso);

	send(UMC_DESCRIPTOR,buffer->datos,buffer->size,0);

	t_header *a_header = malloc(sizeof(t_header));

	char	buffer_header[5];	//Buffer donde se almacena el header recibido

	int		bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
	 		bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

	recv(UMC_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

	char buffer_recv[buffer_header[1]];

	recv(UMC_DESCRIPTOR, buffer_recv, buffer_header[1], 0);

	t_respuesta_cambio_de_proceso *respuesta = malloc(sizeof(t_respuesta_cambio_de_proceso));

	respuesta = (t_respuesta_cambio_de_proceso*)deserealizar_mensaje(buffer_header[0], buffer_recv);

	return respuesta->un_numero;
}

t_dato_en_memoria* get_next_instruction() {
    t_intructions *indice = pcb->instructions_index;

    indice += pcb->program_counter;

    t_intructions *instruccion = indice;

    int pagina = instruccion->start / tamanio_pagina;

    t_dato_en_memoria *result = malloc(sizeof(t_dato_en_memoria));
    result->direccion = malloc(sizeof(t_direccion_virtual_memoria));
    result->direccion->offset = instruccion->start - (pagina * tamanio_pagina);
    result->direccion->pagina = pagina;
    result->size = instruccion->offset;

    return result;
}

t_puntero definirVariable(t_nombre_variable variable) {
	log_trace(trace_log_CPU,"definirVariable");

	t_stack_element *stack_element =list_get(pcb->stack, list_size(pcb->stack) - 1);

	t_variable *new_variable = malloc(sizeof(t_variable));
	new_variable->dato = malloc(sizeof(t_dato_en_memoria));
	new_variable->dato->direccion = malloc(sizeof(t_direccion_virtual_memoria));

	new_variable->id = variable;
	((new_variable->dato)->direccion)->pagina = pcb->stack_free_space_pointer->pagina;
	new_variable->dato->size = sizeof(int);
	((new_variable->dato)->direccion)->offset = pcb->stack_free_space_pointer->offset;

	incrementar_next_free_space(new_variable->dato->size);

	validate_stack_size(new_variable);

	list_add(stack_element->variables, new_variable);

	return convert_to_absolute_offset(new_variable->dato);
}

void validate_stack_size(t_variable *variable) {
    int available_bytes =  pcb->used_pages * tamanio_pagina;
    int absolute_offset = convert_to_absolute_offset(variable->dato) + variable->dato->size;

    if(absolute_offset > available_bytes) {
	pcb->program_finished = 2;
	pthread_exit();
    }
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	log_trace(trace_log_CPU,"obtenerPosicionVariable");

    t_stack_element *stack_element = list_get(pcb->stack, list_size(pcb->stack) - 1);

    int find_variable(t_variable *var) {
	    return var->id == identificador_variable;
    }

    t_variable *variable = list_find(stack_element->variables, (void*)find_variable);

    if(variable == NULL) {
	    pcb->program_finished = 2;
	    pthread_exit();
    }

    return convert_to_absolute_offset(variable->dato);
}

t_valor_variable dereferenciar(t_puntero absolute_offset) {
	log_trace(trace_log_CPU,"dereferenciar");
    t_dato_en_memoria *direccion = convert_to_virtual_address(absolute_offset);

    char* respuesta = ejecutar_lectura_de_dato_con_iteraciones(leer_memoria_de_umc, direccion, tamanio_pagina);
    int valor = 0;
    char *aux = malloc(sizeof(int));
    memcpy(aux, respuesta, sizeof(int));
    valor = (int)*(aux);
    return valor;

}

void go_back_to_previous_stack_element(t_stack_element *current_stack_element) {
    int i;
    for(i=0; i<list_size(current_stack_element->variables); i++) {
	decrementar_next_free_space(sizeof(int));
    }

    int j = list_size(pcb->stack);

    list_remove(pcb->stack, list_size(pcb->stack)-1);

    free_stack_element_memory(current_stack_element);
}

void asignar(t_puntero absolute_offset, t_valor_variable valor) {
	log_trace(trace_log_CPU,"asignar");
    ejecutar_escritura_de_dato_con_iteraciones(convert_to_virtual_address(absolute_offset), (char*) &valor, tamanio_pagina);
}

void retornar(t_valor_variable retorno) {
	log_trace(trace_log_CPU,"retornar");
    t_stack_element *stack_element = list_get(pcb->stack, list_size(pcb->stack)-1);

    asignar(convert_to_absolute_offset(stack_element->valor_retorno), retorno);

    finalizar();
}

void imprimir(t_valor_variable valor_mostrar) {
	log_trace(trace_log_CPU,"Imprimir\n");
    char* print_value = string_itoa(valor_mostrar);

    imprimirTexto(print_value);
}

void imprimirTexto(char* print_value) {
	log_trace(trace_log_CPU,"ImprimirTexto\n");
	int enviado_correctamente = send_text_to_kernel(print_value, string_length(print_value));
    log_trace(trace_log_CPU,"%s\n", print_value);
}

void irALabel(t_nombre_etiqueta nombre_etiqueta) {
	log_trace(trace_log_CPU,"irALabel");
    char* etiqueta_limpia = (char*)nombre_etiqueta;

    if(string_ends_with((char*) nombre_etiqueta, "\n")) {
	etiqueta_limpia = string_substring_until(nombre_etiqueta, string_length((char*)nombre_etiqueta)-1);
    }

    int find_label(t_label_index *label_element) {
	return !strcmp(label_element->name, etiqueta_limpia);
    }

    t_label_index *label = (t_label_index*)list_find(pcb->label_index, find_label);

    pcb->program_counter = label->location;
}

void llamarSinRetorno(t_nombre_etiqueta etiqueta) {
	log_trace(trace_log_CPU,"llamarSinRetorno");
    t_stack_element *stack_element = create_stack_element();
    stack_element->posicion_retorno = pcb->program_counter+1;
    list_add(pcb->stack, stack_element);

    irALabel(etiqueta);
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	log_trace(trace_log_CPU,"llamarConRetorno");
    t_stack_element *stack_element = create_stack_element();
    stack_element->valor_retorno = convert_to_virtual_address(donde_retornar);
    stack_element->posicion_retorno = pcb->program_counter+1;

    list_add(pcb->stack, stack_element);

    irALabel(etiqueta);
}

void finalizar(void) {
	log_trace(trace_log_CPU,"finalizar");
    t_stack_element *stack_element = list_get(pcb->stack, list_size(pcb->stack)-1);

    if(stack_element->posicion_retorno) {
    	pcb->program_counter = stack_element->posicion_retorno;
    }

    go_back_to_previous_stack_element(stack_element);

    if(list_is_empty(pcb->stack)) {
    	pcb->program_finished = 1;
    }
}

int send_text_to_kernel(char* print_value, uint32_t length) {

	t_imprimir_texto_en_cpu *imprimir_en_cpu = malloc(sizeof(t_imprimir_texto_en_cpu));
	imprimir_en_cpu->texto_a_imprimir = print_value;
	t_stream *buffer = serializar_mensaje(132,imprimir_en_cpu);

	int bytes_enviados = send(KERNEL_DESCRIPTOR, buffer->datos, buffer->size, 0);


	free(imprimir_en_cpu);
	free(buffer->datos);
	free(buffer);
	return bytes_enviados;
}

char* leer_memoria_de_umc(t_dato_en_memoria *dato) {

    t_solicitar_bytes_de_una_pagina_a_UMC *pedido = malloc(sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));

    memset(pedido,0,sizeof(t_solicitar_bytes_de_una_pagina_a_UMC));

    pedido->pagina = dato->direccion->pagina;
    pedido->offset = dato->direccion->offset;
    pedido->size = dato->size;
    set_tamanio_pedido(pedido->size);
    t_stream *buffer = (t_stream*)serializar_mensaje(31,pedido);

    send(UMC_DESCRIPTOR, buffer->datos, buffer->size, 0);

    t_header *aHeader = malloc(sizeof(t_header));

    char 	buffer_header[5];	//Buffer donde se almacena el header recibido

    int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
        	bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

    recv(UMC_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

    aHeader = deserializar_header(buffer_header);

    uint8_t tipo = aHeader->tipo;
    uint32_t length = aHeader->length;

    char buffer_recv[length];

    recv(UMC_DESCRIPTOR, buffer_recv, length, 0);

    t_respuesta_bytes_de_una_pagina_a_CPU *respuesta = malloc(sizeof(t_respuesta_bytes_de_una_pagina_a_CPU));

    respuesta = (t_respuesta_bytes_de_una_pagina_a_CPU*)deserealizar_mensaje(tipo, buffer_recv);

    if(respuesta->no_hay_memoria == 1){
    	pcb->program_finished = 58;
    	//ACA NO VA EL PTHREAD EXIT
    }


    return respuesta->bytes_de_una_pagina;
}

int escribir_en_umc(t_dato_en_memoria * dato, char* valor) {

    //char *bytes_de_la_pagina = (char *)valor;

    t_escribir_bytes_de_una_pagina_en_UMC *escritura_bytes = malloc(sizeof(t_escribir_bytes_de_una_pagina_en_UMC));

    memset(escritura_bytes,0,sizeof(t_escribir_bytes_de_una_pagina_en_UMC));

    escritura_bytes->pagina = (dato->direccion)->pagina;
    escritura_bytes->offset = (dato->direccion)->offset;
    escritura_bytes->size= dato->size;
    escritura_bytes->buffer = valor;

    t_stream *buffer = serializar_mensaje(33,escritura_bytes);

    int bytes = send(UMC_DESCRIPTOR, buffer->datos, buffer->size, 0);

    t_header *aHeader = malloc(sizeof(t_header));

    char 	buffer_header[5];	//Buffer donde se almacena el header recibido

    int 	bytes_recibidos_header,	//Cantidad de bytes recibidos en el recv() que recibe el header
            bytes_recibidos;		//Cantidad de bytes recibidos en el recv() que recibe el mensaje completo

    recv(UMC_DESCRIPTOR, buffer_header, 5, MSG_PEEK);

    char buffer_recv[buffer_header[1]];

    recv(UMC_DESCRIPTOR, buffer_recv, buffer_header[1], 0);

    t_respuesta_escribir_bytes_de_una_pagina_en_UMC * respuesta = malloc(sizeof(t_respuesta_escribir_bytes_de_una_pagina_en_UMC));

    respuesta =(t_respuesta_escribir_bytes_de_una_pagina_en_UMC*)deserealizar_mensaje(34, buffer_recv);
    return respuesta->escritura_correcta;
}

t_puntero convert_to_absolute_offset(t_dato_en_memoria* dato) {
    return (tamanio_pagina * dato->direccion->pagina ) + dato->direccion->offset;
}

t_dato_en_memoria* convert_to_virtual_address(t_puntero absolute_offset) {

    t_dato_en_memoria* dato = malloc(sizeof(t_dato_en_memoria));
    dato->direccion = malloc(sizeof(t_direccion_virtual_memoria));

    set_page_and_offset_from_absolute_offset(dato->direccion, absolute_offset);

    dato->size = sizeof(uint32_t);

    return dato;
}

void set_page_and_offset_from_absolute_offset(t_direccion_virtual_memoria* direccion, uint32_t absolute_offset) {
    int new_pages = absolute_offset / tamanio_pagina;
    int new_offset = absolute_offset - (new_pages * tamanio_pagina);

    direccion->pagina = new_pages;
    direccion->offset = new_offset;
}

void incrementar_next_free_space(uint32_t size) {
    int absolute_offset = (tamanio_pagina * pcb->stack_free_space_pointer->pagina ) + pcb->stack_free_space_pointer->offset + size;

    set_page_and_offset_from_absolute_offset( pcb->stack_free_space_pointer, absolute_offset);
};

void decrementar_next_free_space(uint32_t size) {
    int absolute_offset = (tamanio_pagina * pcb->stack_free_space_pointer->pagina ) + pcb->stack_free_space_pointer->offset - size;

    int new_pages = absolute_offset / tamanio_pagina;
    int new_offset = absolute_offset - (new_pages * tamanio_pagina);

    pcb->stack_free_space_pointer->pagina = new_pages;
    pcb->stack_free_space_pointer->offset = new_offset;
};

void free_stack_element_memory(t_stack_element *element) {

    void free_memory(void *variable) {
	free(variable);
    }

    list_destroy_and_destroy_elements(element->variables, free_memory);
    free(element);
};

void set_PCB(t_PCB *new_pcb) {
    pcb = new_pcb;
}

t_PCB* get_PCB() {
    return pcb;
}

void set_tamanio_pagina(uint32_t tamanio) {
    tamanio_pagina = tamanio;
}

t_stack_element* create_stack_element() {
    t_stack_element *stack_element = malloc(sizeof(t_stack_element));
    stack_element->variables = list_create();
    stack_element->posicion_retorno = 0;
    stack_element->valor_retorno = malloc(sizeof(t_dato_en_memoria));
    t_direccion_virtual_memoria * direccion = malloc(sizeof(t_direccion_virtual_memoria));
    direccion->pagina = 0;
    direccion->offset = 0;
    stack_element->valor_retorno->direccion = direccion;

    return stack_element;
}


char* ejecutar_lectura_de_dato_con_iteraciones(void*(*closure_lectura)(t_dato_en_memoria*), t_dato_en_memoria *dato, uint32_t tamanio_pagina) {
    int is_last_page = 0;
    if(is_last_page = (dato->direccion->offset + dato->size <= tamanio_pagina)) {
	return closure_lectura(dato);
    }

    //char *result = malloc(sizeof(char) * dato->size);
    // char *result = malloc(dato->size +1 ); COMENTA EZE
    char *result = malloc(dato->size);
    t_dato_en_memoria *aux_dato = malloc(sizeof(t_dato_en_memoria));
    aux_dato->direccion = malloc(sizeof(t_direccion_virtual_memoria));
    aux_dato->size = dato->size;
    aux_dato->direccion->offset = dato->direccion->offset;
    aux_dato->direccion->pagina = dato->direccion->pagina;
    int remaining_size = dato->size;
    int desplazamiento_acumulado = 0;

    while(remaining_size > 0) {
	if(!is_last_page || remaining_size > tamanio_pagina) {
	    aux_dato->size = tamanio_pagina - aux_dato->direccion->offset;
	} else {
	    aux_dato->size = remaining_size;
	}

	memcpy(result+desplazamiento_acumulado, closure_lectura(aux_dato), aux_dato->size);
	//strcpy(result+desplazamiento_acumulado, closure_lectura(aux_dato));
	desplazamiento_acumulado += aux_dato->size;
	aux_dato->direccion->offset = 0;
	remaining_size -= aux_dato->size;
	aux_dato->direccion->pagina++;

	is_last_page = (aux_dato->direccion->offset + remaining_size < tamanio_pagina);
    }
    free(aux_dato);
    //char endString='\0'; //COMENTA EZE
    //memcpy(result+dato->size,&endString,1);//COMENTA EZE
    return result;
}

int ejecutar_escritura_de_dato_con_iteraciones(t_dato_en_memoria *dato, char* valor, uint32_t tamanio_pagina) {
    int is_last_page = 0;
    if(is_last_page = (dato->direccion->offset + dato->size <= tamanio_pagina)) {
	return escribir_en_umc(dato, valor);
    }

    t_dato_en_memoria *aux_dato = malloc(sizeof(t_dato_en_memoria));
    aux_dato->direccion = malloc(sizeof(t_direccion_virtual_memoria));
    aux_dato->size = dato->size;
    aux_dato->direccion->offset = dato->direccion->offset;
    aux_dato->direccion->pagina = dato->direccion->pagina;

    int remaining_size = dato->size;
    int desplazamiento_acumulado = 0;

    while(remaining_size > 0) {
	if(!is_last_page || remaining_size > tamanio_pagina) {
	    aux_dato->size = tamanio_pagina - aux_dato->direccion->offset;
	} else {
	    aux_dato->size = remaining_size;
	}

	char *data_to_write = malloc(aux_dato->size);
	memcpy(data_to_write, valor+desplazamiento_acumulado, aux_dato->size);

	escribir_en_umc(aux_dato, data_to_write);
	free(data_to_write);

	desplazamiento_acumulado += aux_dato->size;
	aux_dato->direccion->offset = 0;
	remaining_size -= aux_dato->size;
	aux_dato->direccion->pagina++;

	is_last_page = (aux_dato->direccion->offset + remaining_size < tamanio_pagina);
    }

    free(aux_dato);

    return 0;
}

set_umc_socket_descriptor(int socket_descriptor){

    UMC_DESCRIPTOR = socket_descriptor;
}


set_kernel_socket_descriptor(int socket_descriptor){
    KERNEL_DESCRIPTOR = socket_descriptor;
}


void set_quantum(int quantum){
       QUANTUM = quantum;
}
void set_quantum_sleep(int quantum_sleep){
       QUANTUM_SLEEP = quantum_sleep;
}
int ejecutar_pcb(){

       cambiar_contexto(pcb->pid);

       int instruccion_ejecutada = 1;
       while(instruccion_ejecutada <= QUANTUM  && pcb->program_finished == 0){
    	   execute_next_instruction_for_process();
    	   if(pcb->program_finished == 58){
    		   break;
    	   }


    	   log_trace(trace_log_CPU,"Instruccion %d del pid %d ejecutada \n", instruccion_ejecutada, pcb->pid);
		   fflush(stdout);
		   instruccion_ejecutada ++;
		   usleep(1000 * QUANTUM_SLEEP);
       }

      return 0;

}



