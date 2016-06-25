//#ifndef SCHEDULER_H
//#define SCHEDULER_H
//
//	#include <pthread.h>
//	#include <commons/collections/queue.h>
//
//	typedef struct {
//		t_queue* new_state;
//		t_queue* ready_state;
//		t_queue* exit_state;
//		t_queue* block_state;
//		t_queue* execution_state;
//		t_queue* cpus_available;
//		struct t_kernel* kernel;
//		int umc_socket_descriptor;
//	} t_scheduler;
//
//	typedef struct {
//		char* program_code;
//		int console_socket_descriptor;
//	} t_new_program;
//
//	typedef struct {
//		struct t_PCB* pcb;
//		t_scheduler* scheduler;
//	} t_pcb_execution_data;
//
//	sem_t mutex_new, mutex_ready, mutex_exit, mutex_block, mutex_execution,
//		mutex_cpus_available;
//	sem_t sem_new, sem_ready, sem_exit, sem_block, sem_execution, sem_cpus_available;
//	pthread_t new_thread, ready_thread, exit_thread, execution_thread, block_thread;
//
//	t_scheduler* create_scheduler(t_kernel* kernel);
//	void* handle_new(void* scheduler);
//	void* handle_ready(void* scheduler);
//	void* handle_exit(void* scheduler);
//	void* handle_block(void* scheduler);
//	void* handle_execution(void* scheduler);
//	void enqueue_to_ready(t_scheduler* self, t_PCB* pcb);
//	void end_program(t_scheduler* self, t_PCB *pcb);
//	void free_cpu(t_scheduler* self, int cpu);
//#endif
