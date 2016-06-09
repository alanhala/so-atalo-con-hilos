#include "kernel.h"

t_kernel* create_kernel(char* config_file_path) {
	t_config* kernel_config = config_create(config_file_path);
	t_kernel* kernel = malloc(sizeof(t_kernel));
	kernel->pcb_list = list_create();
	kernel->programs_number = 0;
	kernel->console_port = config_get_string_value(kernel_config, "PUERTO_PROG");
	kernel->cpu_port = config_get_string_value(kernel_config, "PUERTO_CPU");
	kernel->quantum = config_get_int_value(kernel_config, "QUANTUM");
	kernel->quantum_sleep = config_get_int_value(kernel_config, "QUANTUM_SLEEP");
	kernel->stack_size = config_get_int_value(kernel_config, "STACK_SIZE");
	return kernel;
}

t_PCB* initialize_program(t_kernel* self, char* program) {
	t_metadata_program* metadata = metadata_desde_literal(program);
	t_PCB* pcb = create_pcb(self, metadata);
	self->programs_number += 1;
	return pcb;
}

t_PCB* create_pcb(t_kernel* kernel, t_metadata_program* metadata) {
	t_PCB* pcb = malloc(sizeof(t_PCB));
	pcb->pid = kernel->programs_number;
	pcb->program_counter = metadata->instruccion_inicio;
	pcb->stack_index = list_create();
	pcb->stack_size = kernel->stack_size;
	pcb->instructions_index = metadata->instrucciones_serializado;
	pcb->instructions_size = metadata->instrucciones_size;
//	TODO CACLULAR INICIO DEL STACK POINTER
	return pcb;
}
