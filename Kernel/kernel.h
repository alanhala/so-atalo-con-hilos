#ifndef KERNEL_H
#define	KERNEL_H

	#include <stdint.h>

	#include <commons/collections/list.h>
	#include <commons/config.h>
	#include <parser/metadata_program.h>

	typedef struct {
		uint32_t page;
		uint32_t offset;
	} t_virtual_address;

	typedef struct {
		char* name;
		uint32_t location;
	} t_label_index;

	typedef struct {
		t_list* pcb_list;
		uint32_t programs_number;
		char* console_port;
		char* cpu_port;
		uint32_t quantum;
		uint32_t quantum_sleep;
		char* io_ids;
		char* sem_ids;
		t_list* shared_vars;
		uint32_t stack_size;
	} t_kernel;

	typedef struct {
		uint32_t pid;
		char * state; //no serializar
		int cpu_socket_descriptor; //no serializar
		int console_socket_descriptor; // no serializar
		uint32_t program_counter;
		t_list* stack_index;
		t_virtual_address* stack_last_address;
		uint32_t stack_pointer;
		uint32_t stack_size;
		uint32_t used_pages;
		uint32_t instructions_size;
		t_intructions* instructions_index;
		t_list* label_index;
		uint32_t program_finished;
	} t_PCB;


	typedef struct {
	    uint32_t size;
	    t_virtual_address *direccion;
	} t_dato_en_memoria;

	typedef struct {
		t_list *variables;
		uint32_t posicion_retorno;
		t_dato_en_memoria *valor_retorno;
	} t_stack_element;

	typedef struct {
		char* name;
		uint32_t value;
	} t_shared_variable;

	typedef struct {
		t_nombre_variable id;
    		t_dato_en_memoria * dato;
	}t_variable;

	t_PCB* create_pcb(t_kernel* kernel, char* program);

	t_kernel* create_kernel(char* config_file_path);

	t_PCB* initialize_program(t_kernel* self, char* program);

	t_virtual_address* get_stack_address(char* program);

	t_list* get_label_index(t_metadata_program* metadata);

	t_label_index* create_label_index(char* label_name, int label_location);

	uint32_t get_shared_var_value(t_kernel* self, char* shared_variable);

	uint32_t update_shared_var_value(t_kernel* self, char* variable_name, uint32_t value);
#endif
