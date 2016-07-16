#ifndef KERNEL_H
#define	KERNEL_H

	#include <stdint.h>
	#include <semaphore.h>
	#include <commons/collections/list.h>
	#include <commons/config.h>
	#include <parser/metadata_program.h>
	#include <commons/collections/queue.h>

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
		t_list* shared_vars;
		uint32_t stack_size;
		t_list* io_list;
		t_list* semaphores;
	} t_kernel;

	typedef struct {
		t_queue* new_state;
		t_queue* ready_state;
		t_queue* exit_state;
		t_list* block_state;
		t_queue* execution_state;
		t_queue* cpus_available;
		t_kernel* kernel;
		int umc_socket_descriptor;
		t_list* semaphores_list;
		t_list* io_list;
		t_list* closed_consoles;
	} t_scheduler;

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

	typedef struct {
		char* name;
		uint32_t sleep;
	}t_io;

	typedef struct {
		char* id;
		int32_t value;
	} t_semaphore;

	typedef struct {
		char* id;
		t_queue* blocked_pids;
	} t_sem_blocked_queue;

	typedef struct {
		char* program_code;
		int console_socket_descriptor;
	} t_new_program;

	typedef struct {
		struct t_PCB* pcb;
		t_scheduler* scheduler;
	} t_pcb_execution_data;

	typedef struct {
		t_io* io;
		sem_t resources;
		t_queue* blocked_pids;
	} t_io_blocked_queue;

	typedef struct {
		uint32_t pid;
		uint32_t operations_count;
	} t_io_blocked_process_data;

	sem_t mutex_new, mutex_ready, mutex_exit, mutex_block, mutex_execution,
		mutex_cpus_available;
	sem_t sem_new, sem_ready, sem_exit, sem_execution, sem_cpus_available;
	pthread_t new_thread, ready_thread, exit_thread, execution_thread;
	t_scheduler* scheduler;

	t_PCB* create_pcb(t_kernel* kernel, char* program);

	t_kernel* create_kernel(char* config_file_path);

	t_PCB* initialize_program(t_kernel* kernel, char* program);

	t_virtual_address* get_stack_address(char* program);

	t_list* get_label_index(t_metadata_program* metadata);

	t_label_index* create_label_index(char* label_name, int label_location);

	uint32_t get_shared_var_value(t_kernel* kernel, char* shared_variable);

	uint32_t update_shared_var_value(t_kernel* kernel, char* variable_name, uint32_t value);

	int32_t io_call(t_kernel* kernel, char* io_name, int times, t_PCB* pcb);

	int32_t wait_ansisop(t_kernel* kernel, char* sem_id, t_PCB* pcb);

	int32_t signal_ansisop(t_kernel* kernel, char* sem_id);

	t_scheduler* create_scheduler(t_kernel* kernel);
	void* handle_new(void* scheduler);
	void* handle_ready(void* scheduler);
	void* handle_exit(void* scheduler);
	void* handle_execution(void* scheduler);
	void enqueue_to_ready(t_scheduler* scheduler, t_PCB* pcb);
	void end_program(t_scheduler* scheduler, t_PCB *pcb);
	void free_cpu(t_scheduler* scheduler, int cpu);
	void wait_block_process(t_scheduler* scheduler, char* sem_id, t_PCB* pcb);
	void signal_unblock_process(t_scheduler* scheduler, char* sem_id);
	void set_page_size(uint32_t size);
	uint32_t get_used_pages(t_PCB *pcb, uint32_t stack_size);
	void* handle_io_queue(void* io_attr);
	void handle_io_operation(t_scheduler* scheduler, char* io_name, int times, t_PCB* pcb);
	uint32_t check_blocked_pcb(t_scheduler* scheduler, int console_socket_descriptor);
#endif
