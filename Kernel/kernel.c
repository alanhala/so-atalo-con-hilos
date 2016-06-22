#include "kernel.h"

t_list* load_shared_vars(char** shared_vars_list) {
	t_list* shared_variables = list_create();
	int i = 0;
	while(*(shared_vars_list + i) != NULL) {
		t_shared_variable* shared_variable = malloc(sizeof(t_shared_variable));
		shared_variable->name = *(shared_vars_list + i);
		shared_variable->value = 0;
		list_add(shared_variables, shared_variable);
		i++;
	}
	return shared_variables;
}

t_list* load_input_output_list(char** name_list, char** sleep_values) {
	t_list* io_list = list_create();
	int i = 0;
	while(*(name_list + i) != NULL) {
		t_io* io = malloc(sizeof(t_io));
		io->name = *(name_list + i);
		io->sleep = atoi(*(sleep_values + i));
		sem_init(&io->resources, 0, 1);
		list_add(io_list, io);
		i++;
	}
	return io_list;
}

t_list* load_semaphores(char** semaphores_name, char** semaphores_init) {
	t_list* semaphores = list_create();
	int i = 0;
	while(*(semaphores_name + i) != NULL) {
		t_semaphore* semaphore = malloc(sizeof(t_semaphore));
		semaphore->id = *(semaphores_name + i);
		semaphore->value = atoi(*(semaphores_init + i));
		list_add(semaphores, semaphore);
		i++;
	}
	return semaphores;
}

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
	kernel->shared_vars = load_shared_vars(config_get_array_value(kernel_config, "SHARED_VARS"));
	kernel->io_list = load_input_output_list(config_get_array_value(kernel_config, "IO_IDS"),
			config_get_array_value(kernel_config, "IO_SLEEP"));
	kernel->semaphores = load_semaphores(config_get_array_value(kernel_config, "SEM_IDS"),
			config_get_array_value(kernel_config, "SEM_INIT"));
	scheduler = create_scheduler(kernel);
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
	pcb->used_pages = 300; // ESTO LO HARCODEO EZE. TIENE QE CALCULARSE A PARTIR DEL PROGRAMA Y EL STAACK. validar con mati
	pcb->stack_last_address = get_stack_address(program);
	pcb->label_index = get_label_index(metadata);
	pcb->program_finished = 0;
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

uint32_t get_shared_var_value(t_kernel* self, char* variable_name) {
	int same_variable(t_shared_variable* shared_variable) {
		if (strcmp(shared_variable->name, variable_name) == 0)
			return 1;
		else
			return 0;
	}
	t_shared_variable* shared_variable = list_find(self->shared_vars, (void*) same_variable);
	return shared_variable->value;
}

uint32_t update_shared_var_value(t_kernel* self, char* variable_name, uint32_t value) {
	int same_variable(t_shared_variable* shared_variable) {
		if (strcmp(shared_variable->name, variable_name) == 0)
			return 1;
		else
			return 0;
	}
	t_shared_variable* shared_variable = list_find(self->shared_vars, (void*) same_variable);
	shared_variable->value = value;
	return 0;
}

uint32_t io_call(t_kernel* self, char* io_name, int times) {
	int same_io(t_io* io) {
		if (strcmp(io->name, io_name) == 0)
			return 1;
		else
			return 0;
	}
	t_io* io = list_find(self->io_list, (void*) same_io);
	sem_wait(&io->resources);
	usleep(io->sleep * times * 1000); // milisegundos a microsegundos
	sem_post(&io->resources);
	return 0;
}

int32_t wait_ansisop(t_kernel* kernel, char* sem_id, t_PCB* pcb) {
	int same_sem(t_semaphore* sem) {
		if (strcmp(sem->id, sem_id) == 0)
			return 1;
		else
			return 0;
	}
	t_semaphore* semaphore = list_find(kernel->semaphores, (void*) same_sem);
	semaphore->value--; // TODO ver si hace falta poner semaforo
	if (semaphore->value < 0) {
		block_process(scheduler, semaphore->id, pcb);
		return -1;
	}
	else
		return 0;

}

int32_t signal_ansisop(t_kernel* kernel, char* sem_id) {
	int same_sem(t_semaphore* sem) {
		if (strcmp(sem->id, sem_id) == 0)
			return 1;
		else
			return 0;
	}
	t_semaphore* semaphore = list_find(kernel->semaphores, (void*) same_sem);
	semaphore->value++; // TODO ver si hace falta poner semaforo
	if (semaphore->value <= 0) {
		unblock_process(scheduler, semaphore->id);
	}
	return 0;
}
