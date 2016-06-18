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
	t_PCB* pcb = create_pcb(self, program);
	self->programs_number += 1;
	return pcb;
}

t_PCB* create_pcb(t_kernel* kernel, char* program) {
	t_metadata_program* metadata = metadata_desde_literal(program);
	t_PCB* pcb = malloc(sizeof(t_PCB));
	pcb->pid = kernel->programs_number;
	pcb->program_counter = metadata->instruccion_inicio;
	pcb->stack_index = list_create();
	t_stack_element * stack_element = malloc(sizeof(t_stack_element));
	stack_element->variables = list_create();
	stack_element->posicion_retorno = 0;
	stack_element->valor_retorno = malloc(sizeof(t_dato_en_memoria));
	list_add(pcb->stack_index, stack_element);

	pcb->stack_size = kernel->stack_size;
	pcb->instructions_index = metadata->instrucciones_serializado;
	pcb->instructions_size = metadata->instrucciones_size;
	pcb->used_pages = 100; // ESTO LO HARCODEO EZE. TIENE QE CALCULARSE A PARTIR DEL PROGRAMA Y EL STAACK. validar con mati
	pcb->stack_last_address = get_stack_address(program);
	pcb->label_index = get_label_index(metadata);
	return pcb;
}

t_virtual_address* get_stack_address(char* program) {
	t_virtual_address* address = malloc(sizeof(t_virtual_address));
	int page = strlen(program) / 5; // TODO sacar el dato hardcodeado por el size de la pagina
	int offset = strlen(program) - page * 5; // TODO sacar de aca tambien
	address->page = page;
	address->offset = offset;
	return address;
}

t_list* get_label_index(t_metadata_program* metadata) {
	t_list* label_list = list_create();
	int i, name_size;
	int first_label_location = 0;
	for (i = 0; i < (metadata->cantidad_de_etiquetas +
			metadata->cantidad_de_funciones); i++) {
		name_size = 0;
		while(*(metadata->etiquetas + first_label_location + name_size) != '\0') {
			name_size ++;
		}
		name_size ++;
		char* label_name = malloc(name_size);
		memcpy(label_name, metadata->etiquetas + first_label_location, name_size);
		int label_location;
		memcpy(&label_location, metadata->etiquetas + first_label_location
				+ name_size, 4);
		t_label_index* label_index = create_label_index(label_name, label_location);
		list_add(label_list, label_index);
		first_label_location += (name_size + 4);
	}
	return label_list;
}

t_label_index* create_label_index(char* label_name, int label_location) {
	t_label_index* label_index = malloc(sizeof(t_label_index));
	label_index->name = label_name;
	label_index->location = label_location;
	return label_index;
}
