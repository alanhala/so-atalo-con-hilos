#include "kernel_communication.h"
#include "protocoloKernel.h"
#include <commons/log.h>
extern t_log *kernel_trace;

int validate_console_connection(int socket_fd){
		int error = 0;
		socklen_t len = sizeof (error);
		int retval = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);


		if (retval != 0) {
			/* there was a problem getting the error code */
			//fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
			return 1;
		}

		if (error != 0) {
			/* socket has a non zero error status */
			fprintf(stderr, "socket error: %s\n", strerror(error));
			return 1;
		}
//		int isopen =isfdtype(socket_fd, SOCK_STREAM);
//		if (isopen ! 1)
//			return 1;

		return 0;
}

int start_program_in_umc(int umc_socket_descriptor, int pid, int cantidad_paginas_requeridas, char* codigo) {

	   t_inicio_de_programa_en_UMC *iniciar_programa_en_UMC = malloc(sizeof(t_inicio_de_programa_en_UMC));
	   memset(iniciar_programa_en_UMC,0,sizeof(t_inicio_de_programa_en_UMC));

	   iniciar_programa_en_UMC->process_id = pid;
	   iniciar_programa_en_UMC->cantidad_de_paginas = cantidad_paginas_requeridas;
	   iniciar_programa_en_UMC->codigo_de_programa = codigo;

	   t_stream *buffer = malloc(sizeof(t_stream));

	   buffer = serializar_mensaje(61,iniciar_programa_en_UMC);

	   free(iniciar_programa_en_UMC);

	   int bytes_enviados = send(umc_socket_descriptor,buffer->datos,buffer->size,0);
	   free(buffer->datos);
	   free(buffer);

	   char buffer_header[5];

	   int bytes_header = recv(umc_socket_descriptor,buffer_header,5,MSG_PEEK);

	   char buffer_recv[buffer_header[1]];

	   int bytes_recibidos = recv(umc_socket_descriptor,buffer_recv,buffer_header[1],0);

	   t_respuesta_iniciar_programa_en_UMC *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_UMC));
	   memset(respuesta,0,sizeof(t_respuesta_iniciar_programa_en_UMC));
	   respuesta = deserealizar_mensaje(buffer_header[0],buffer_recv);
	   int resultado = respuesta->respuesta_correcta;
	   free(respuesta);
	   return resultado;
}

void* handle_pcb_execution(void* data_to_cast) {
	t_pcb_execution_data* data = (t_pcb_execution_data*) data_to_cast;
	t_scheduler* scheduler = data->scheduler;
	t_PCB* pcb = data->pcb;
	t_kernel* kernel = scheduler->kernel;

	t_PCB_serializacion * pcb_serializacion = adaptar_pcb_a_serializar(pcb, kernel);
	pcb_serializacion->mensaje = 0;
	pcb_serializacion->valor_mensaje = "";
	pcb_serializacion->cantidad_operaciones = 0;
	pcb_serializacion->valor_de_la_variable_compartida = 0;
	pcb_serializacion->resultado_mensaje = 0;
	t_stream *buffer = serializar_mensaje(121,pcb_serializacion);

	int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
	free(buffer->datos);
	free(buffer);
	if (bytes_enviados == 0 || bytes_enviados == -1){
		log_trace(kernel_trace,"PID %d :Cpu desconectada, se finaliza el proceso\n", pcb->pid);
		pcb->program_finished = 34; //CPU DESCONECATADA
		end_program(scheduler, pcb);
		pthread_exit(1);
	}
	log_trace(kernel_trace,"PID %d : Pcb enviado a cpu\n", pcb->pid);

	while(1){

			t_header *un_header = malloc(sizeof(t_header));
			char buffer_header[5];

			int	bytes_recibidos_header,
				bytes_recibidos;

			bytes_recibidos_header = recv(pcb->cpu_socket_descriptor,buffer_header,5,MSG_PEEK);
			if(bytes_recibidos_header == 0 ){
				log_trace(kernel_trace,"PID %d :Cpu desconectada, se finaliza el proceso\n", pcb->pid);
				pcb->program_finished = 34; //CPU DESCONECATADA
				end_program(scheduler, pcb);
				break;
			}


			un_header = deserializar_header(buffer_header);

			uint8_t tipo = un_header->tipo;
			uint32_t length = un_header->length;
			free(un_header);

			char buffer_recibidos[length];

			if(tipo == 132){

				int bytes_recibidos = recv(pcb->cpu_socket_descriptor,buffer_recibidos,length,0);

				if(bytes_recibidos == 0){
					log_trace(kernel_trace,"PID %d :Cpu desconectada, se finaliza el proceso\n", pcb->pid);
					pcb->program_finished = 34; //CPU DESCONECATADA
					end_program(scheduler, pcb);
					break;
				}

				if(validate_console_connection(pcb->console_socket_descriptor) == 1){

						close(pcb->console_socket_descriptor);
//						int umc_finalizado = end_program_umc(pcb, self->umc_socket_descriptor);
//						free(pcb);
				}else
				{
					int bytes_sent = send(pcb->console_socket_descriptor,buffer_recibidos,length,0);
					log_trace(kernel_trace,"PID %d : Envio imprimir texto a consola\n", pcb->pid);
				}
			}
			if(tipo == 121){

				int bytes_recibidos = recv(pcb->cpu_socket_descriptor,buffer_recibidos,length,0);
				if(bytes_recibidos == 0){
					log_trace(kernel_trace,"PID %d :Cpu desconectada, se finaliza el proceso\n", pcb->pid);
					pcb->program_finished = 34; //CPU DESCONECATADA
					end_program(scheduler, pcb);
					break;
				}


				t_PCB_serializacion *unPCB = deserealizar_mensaje(121,buffer_recibidos);

				actualizar_pcb_serializado(pcb, unPCB);

				if(unPCB->mensaje == 1){
					uint32_t valor_variable =get_shared_var_value(kernel, unPCB->valor_mensaje);

					t_PCB_serializacion * pcb_serializacion = adaptar_pcb_a_serializar(pcb, kernel);
					pcb_serializacion->mensaje = 0;
					pcb_serializacion->valor_mensaje = "";
					pcb_serializacion->cantidad_operaciones = 0;
					pcb_serializacion->valor_de_la_variable_compartida = 0;
					pcb_serializacion->resultado_mensaje = valor_variable;
					t_stream *buffer = serializar_mensaje(121,pcb_serializacion);

					int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
					if(bytes_enviados == 0){
						log_trace(kernel_trace,"PID %d :Cpu desconectada, se finaliza el proceso\n", pcb->pid);
						pcb->program_finished = 34; //CPU DESCONECATADA
						end_program(scheduler, pcb);
						break;
					}
					free(buffer->datos);
					free(buffer);
				}else if(unPCB->mensaje ==2){
					//aca hay que renombrar el cantidad de operaciones ya que no imagine todos los casos.
					//estoy reutilizadno el campo para no serializar algo mas
					uint32_t resultado =update_shared_var_value(kernel, unPCB->valor_mensaje, unPCB->valor_de_la_variable_compartida);
					t_PCB_serializacion * pcb_serializacion = adaptar_pcb_a_serializar(pcb, kernel);
					pcb_serializacion->mensaje = 0;
					pcb_serializacion->valor_mensaje = "";
					pcb_serializacion->cantidad_operaciones = 0;
					pcb_serializacion->valor_de_la_variable_compartida =0;
					pcb_serializacion->resultado_mensaje = resultado;
					t_stream *buffer = serializar_mensaje(121,pcb_serializacion);

					int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
					if (bytes_enviados == -1 || bytes_enviados == 0){
						log_trace(kernel_trace,"PID %d :Cpu desconectada, se finaliza el proceso\n", pcb->pid);
						pcb->program_finished = 34; //CPU DESCONECATADA
						end_program(scheduler, pcb);
						break;
					}
					free(buffer->datos);
					free(buffer);
				} else if(unPCB->mensaje == 3) {
				    int cpu = pcb->cpu_socket_descriptor;
				    if(pcb->program_finished == 1 || pcb->program_finished == 2 || pcb->program_finished == 58)
				    	end_program(scheduler, pcb);
				    else if (pcb->program_finished == 6)
				    	handle_io_operation(scheduler, unPCB->valor_mensaje, unPCB->cantidad_operaciones, pcb);
				    else
				    	enqueue_to_ready(scheduler, pcb);
				    if (!unPCB->cpu_unplugged)
				    	free_cpu(scheduler, cpu);
				    break; // TODO REVISAR
				} else if(unPCB->mensaje == 4) {
					int resultado = wait_ansisop(kernel, unPCB->valor_mensaje, pcb);
					t_PCB_serializacion * pcb_serializacion = adaptar_pcb_a_serializar(pcb, kernel);
					if (resultado == -1) {
						pcb_serializacion->program_finished = 5;
					}
					pcb_serializacion->mensaje = 0;
					pcb_serializacion->valor_mensaje = "";
					pcb_serializacion->cantidad_operaciones = 0;
					pcb_serializacion->valor_de_la_variable_compartida =0;
					pcb_serializacion->resultado_mensaje = 0;
					t_stream *buffer = serializar_mensaje(121,pcb_serializacion);

					int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
					if (bytes_enviados == -1 || bytes_enviados == 0){
						log_trace(kernel_trace,"PID %d :Cpu desconectada, se finaliza el proceso\n", pcb->pid);
						pcb->program_finished = 34; //CPU DESCONECATADA
						end_program(scheduler, pcb);
						break;
					}
					free(buffer->datos);
					free(buffer);
					if (resultado == -1)
						break;
				} else if (unPCB->mensaje == 5) {
					signal_ansisop(kernel, unPCB->valor_mensaje);
				} else if (unPCB->mensaje == 6) {
					int resultado = io_call(kernel, unPCB->valor_mensaje, unPCB->cantidad_operaciones, pcb);
					t_PCB_serializacion * pcb_serializacion = adaptar_pcb_a_serializar(pcb, kernel);
					pcb_serializacion->mensaje = 0;
					pcb_serializacion->valor_mensaje = "";
					pcb_serializacion->cantidad_operaciones = 0;
					pcb_serializacion->valor_de_la_variable_compartida =0;
					pcb_serializacion->resultado_mensaje = 0;
					pcb_serializacion->program_finished = 6;
					t_stream *buffer = serializar_mensaje(121,pcb_serializacion);

					int bytes_enviados = send(pcb->cpu_socket_descriptor, buffer->datos, buffer->size, 0);
					if (bytes_enviados == -1 || bytes_enviados == 0){
						log_trace(kernel_trace,"PID %d :Cpu desconectada, se finaliza el proceso\n", pcb->pid);
						pcb->program_finished = 34; //CPU DESCONECATADA
						end_program(scheduler, pcb);
						break;
					}
					free(buffer->datos);
					free(buffer);
				}
			}
	}
	pthread_exit(1);
}

int end_program_umc(t_PCB *pcb, int umc_socket_descriptor) {
		t_finalizar_programa_en_UMC *finalizar_programa_en_UMC = malloc(sizeof(t_finalizar_programa_en_UMC));
		memset(finalizar_programa_en_UMC,0,sizeof(t_finalizar_programa_en_UMC));



		finalizar_programa_en_UMC->process_id = pcb->pid;

		t_stream *buffer = malloc(sizeof(t_stream));

		buffer = serializar_mensaje(63,finalizar_programa_en_UMC);
		free(finalizar_programa_en_UMC);
		int bytes_enviados = send(umc_socket_descriptor,buffer->datos,buffer->size,0);
		free(buffer->datos);
		free(buffer);
		char buffer_header[5];

		int bytes_header = recv(umc_socket_descriptor,buffer_header,5,MSG_PEEK);

		char buffer_recv[buffer_header[1]];

		int bytes_recibidos = recv(umc_socket_descriptor,buffer_recv,buffer_header[1],0);

		t_respuesta_finalizar_programa_en_UMC *respuesta_finalizar_prog_UMC = malloc(sizeof(t_respuesta_finalizar_programa_en_UMC));

		memset(respuesta_finalizar_prog_UMC,0,sizeof(t_respuesta_finalizar_programa_en_UMC));

		respuesta_finalizar_prog_UMC = deserealizar_mensaje(64,buffer_recv);

		int resultado = respuesta_finalizar_prog_UMC->respuesta_correcta;
		free(respuesta_finalizar_prog_UMC);
		return respuesta_finalizar_prog_UMC->respuesta_correcta;
}

int end_program_console(t_PCB *pcb) {
	t_finalizar_programa_en_consola * finalizar_consola = malloc(sizeof(t_finalizar_programa_en_consola));
	memset(finalizar_consola,0,sizeof(t_finalizar_programa_en_consola));

	finalizar_consola->motivo = pcb->program_finished;
	t_stream *buffer = malloc(sizeof(t_stream));

	buffer = serializar_mensaje(133,finalizar_consola);
	free(finalizar_consola);

	if(validate_console_connection(pcb->console_socket_descriptor) == 1){
						close(pcb->console_socket_descriptor);
	}else
	{
		send(pcb->console_socket_descriptor,buffer->datos,buffer->size,0);
	}
	free(buffer->datos);
	free(buffer);
	return 0;
}

t_PCB_serializacion* adaptar_pcb_a_serializar(t_PCB* pcb, t_kernel* kernel) {
	t_PCB_serializacion * pcb_serializacion = malloc(sizeof(t_PCB_serializacion));
	pcb_serializacion->instructions_index = pcb->instructions_index;
	pcb_serializacion->instructions_size = pcb->instructions_size;
	pcb_serializacion->label_index = pcb->label_index; //todo chequear que alan lo este inicializando
	pcb_serializacion->pid = pcb->pid;
	pcb_serializacion->program_counter = pcb->program_counter;
	pcb_serializacion->program_finished = 0; //TODO revisar que valor le pongo
	pcb_serializacion->quantum = kernel->quantum;
	pcb_serializacion->quantum_sleep = kernel->quantum_sleep;
	pcb_serializacion->stack_index = pcb->stack_index;
	pcb_serializacion->stack_last_address = pcb->stack_last_address;
	pcb_serializacion->stack_size = pcb->stack_size;
	pcb_serializacion->used_pages = pcb->used_pages;

	return pcb_serializacion;
}

void actualizar_pcb_serializado(t_PCB* pcb, t_PCB_serializacion* pcb_serializacion) {
	pcb->instructions_index = pcb_serializacion->instructions_index;
	pcb->instructions_size = pcb_serializacion->instructions_size;
	pcb->label_index = pcb_serializacion->label_index;
	pcb->pid = pcb_serializacion->pid;
	pcb->program_counter = pcb_serializacion->program_counter;
	pcb->program_finished = pcb_serializacion->program_finished;
	pcb->stack_index = pcb_serializacion->stack_index;
	pcb->stack_last_address = pcb_serializacion->stack_last_address;
	pcb->stack_size = pcb_serializacion->stack_size;
	pcb->used_pages = pcb_serializacion->used_pages;
}
