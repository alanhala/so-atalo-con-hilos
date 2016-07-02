#ifndef KERNEL_COMMUNICATION_H
#define KERNEL_COMMUNICATION_H
	#include <sys/socket.h>
	#include <pthread.h>
	#include "kernel.h"
	#include "protocoloKernel.h"

	int start_program_in_umc(int umc_socket_descriptor, int pid, int cantidad_paginas_requeridas, char* codigo);
	void* handle_pcb_execution(void* data_to_cast);
	int end_program_umc(t_PCB *pcb, int umc_socket_descriptor);
	int end_program_console(t_PCB *pcb);
	t_PCB_serializacion* adaptar_pcb_a_serializar(t_PCB* pcb, t_kernel* kernel);
	void actualizar_pcb_serializado(t_PCB *pcb, t_PCB_serializacion *pcb_serializacion);

	int validate_console_connection(int socket_fd);

#endif
