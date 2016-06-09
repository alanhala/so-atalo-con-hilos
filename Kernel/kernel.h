#ifndef KERNEL_H
#define	KERNEL_H

	#include <stdint.h>

	#include <commons/collections/list.h>
	#include <commons/config.h>
	#include <parser/metadata_program.h>

	typedef struct {
		t_list* pcb_list;
		uint32_t programs_number;
		char* console_port;
		char* cpu_port;
		uint32_t quantum;
		uint32_t quantum_sleep;
		char* io_ids;
		char* sem_ids;
		char* shared_vars;
		uint32_t stack_size;
	} t_kernel;

	typedef struct {
		uint32_t pid;
		char * state; //no serializar
		uint32_t program_counter;
		t_list* stack_index;
		uint32_t stack_pointer;
		uint32_t stack_size;
		uint32_t used_pages;
		uint32_t instructions_size;
		t_intructions* instructions_index;
//		uint32_t labels_size;
	//		t_indice_etiqueta
	} t_PCB;

	t_PCB* create_pcb(t_kernel* kernel, t_metadata_program* metadata);

	t_kernel* create_kernel(char* config_file_path);

	t_PCB* initialize_program(t_kernel* self, char* program);

#endif
