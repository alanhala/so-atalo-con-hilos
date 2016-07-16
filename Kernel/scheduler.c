#include "kernel_communication.h"
#include <commons/log.h>
extern t_log *kernel_trace;

uint32_t check_closed_console(t_scheduler* scheduler, int console_descriptor) {
	int same_descriptor(int descriptor) {
		return descriptor == console_descriptor;
	}
	int descriptor = list_remove_by_condition(scheduler->closed_consoles, (void*) same_descriptor);
	if (descriptor == NULL)
		return 0;
	else
		return 1;
}

void initialize_semaphores() {
	sem_init(&mutex_new, 0, 1);
	sem_init(&mutex_ready, 0, 1);
	sem_init(&mutex_execution, 0, 1);
	sem_init(&mutex_block, 0, 1);
	sem_init(&mutex_exit, 0, 1);
	sem_init(&mutex_cpus_available, 0, 1);

	sem_init(&sem_new, 0, 0);
	sem_init(&sem_ready, 0,0);
	sem_init(&sem_execution, 0, 0);
	sem_init(&sem_exit, 0, 0);
	sem_init(&sem_cpus_available, 0, 0);
}

void create_states_threads(t_scheduler* self) {
	pthread_create(&new_thread, NULL, &handle_new, (void*) self);
	pthread_create(&ready_thread, NULL, &handle_ready, (void*) self);
	pthread_create(&execution_thread, NULL, &handle_execution, (void*) self);
	pthread_create(&exit_thread, NULL, &handle_exit, (void*) self);
}

t_list* create_semaphores_list(t_kernel* kernel) {
	t_list* semaphores_list = list_create();
	int i;
	for(i = 0; i < kernel->semaphores->elements_count; i++) {
		t_sem_blocked_queue* sem_blocked = malloc(sizeof(t_sem_blocked_queue));
		t_semaphore* semaphore = list_get(kernel->semaphores, i);
		sem_blocked->id = semaphore->id;
		sem_blocked->blocked_pids = queue_create();
		list_add(semaphores_list, sem_blocked);
	}
	return semaphores_list;
}

t_list* create_io_list(t_kernel* kernel) {
	t_list* io_list = list_create();
	int i;
	for(i = 0; i < kernel->io_list->elements_count; i++) {
		t_io_blocked_queue* io_queue = malloc(sizeof(t_io_blocked_queue));
		t_io* io = list_get(kernel->io_list, i);
		io_queue->io = io;
		sem_init(&io_queue->resources, 0, 0);
		io_queue->blocked_pids = queue_create();
		list_add(io_list, io_queue);
		pthread_t io_thread;
		pthread_create(&io_thread, NULL, &handle_io_queue, (void*) io_queue);
	}
	return io_list;
}

t_scheduler* create_scheduler(t_kernel* kernel) {
	t_scheduler* self = malloc(sizeof(t_scheduler));
	self->new_state = queue_create();
	self->ready_state = queue_create();
	self->execution_state = queue_create();
	self->block_state = list_create();
	self->exit_state = queue_create();
	self->cpus_available = queue_create();
	self->semaphores_list = create_semaphores_list(kernel);
	self->io_list = create_io_list(kernel);
	self->kernel = kernel;
	self->closed_consoles = list_create();
	initialize_semaphores();
	create_states_threads(self);
	return self;
}

void* handle_new(void* scheduler) {
	t_scheduler* self = (t_scheduler*) scheduler;
	while (1) {
		sem_wait(&sem_new);
		sem_wait(&mutex_new);
		t_new_program * new_program = queue_pop(self->new_state);
		sem_post(&mutex_new);

		t_PCB *pcb = initialize_program(self->kernel, new_program->program_code);
		pcb->console_socket_descriptor = new_program->console_socket_descriptor;
		int result = start_program_in_umc(self->umc_socket_descriptor,
				pcb->pid, pcb->used_pages, new_program->program_code);
		if (result == -1 || result == 9) {
			log_trace(kernel_trace,"PID %d : No pudo iniciarse el programa correctamente\n", pcb->pid);
			pcb->program_finished = 7;
			//end_program(scheduler, pcb);
			int consola_finalizado = end_program_console(pcb); //console_finalizado no usarlo para nada es valor 0

			free(pcb);// lo libero directamente creo q no es necesario hacer cola de exit

		}
		else {
			log_trace(kernel_trace,"PID %d : Inicio de programa correcto \n", pcb->pid);


			enqueue_to_ready(self, pcb);
		}
	}
}

void* handle_ready(void* scheduler) {
	t_scheduler* self = (t_scheduler*) scheduler;
	while (1) {
		sem_wait(&sem_ready);
		sem_wait(&mutex_ready);
		t_PCB *pcb  = queue_pop(self->ready_state);
		sem_post(&mutex_ready);
		if (check_closed_console(self, pcb->console_socket_descriptor) == 1) {
			end_program_umc(pcb, self->umc_socket_descriptor);
			free(pcb);
			continue;
		}
		sem_wait(&sem_cpus_available); //espero tener una cpu disponible

		if(validate_console_connection(pcb->console_socket_descriptor) == 1){
			sem_post(&sem_cpus_available);
			close(pcb->console_socket_descriptor);
			int umc_finalizado = end_program_umc(pcb, self->umc_socket_descriptor);
			free(pcb);
			continue;
		}



		sem_wait(&mutex_execution);
		pcb->state = "Ejecutando";
		log_trace(kernel_trace,"Ejecutando: %d\n", pcb->pid);
		queue_push(self->execution_state, pcb);
		sem_post(&mutex_execution);
		sem_post(&sem_execution);
	}
}

void* handle_execution(void* scheduler) {
	t_scheduler* self = (t_scheduler*) scheduler;
	while(1){
		sem_wait(&sem_execution);
		sem_wait(&mutex_execution);
		t_PCB* pcb  = queue_pop(self->execution_state);
		sem_post(&mutex_execution);

		if (check_closed_console(self, pcb->console_socket_descriptor) == 1) {
			end_program_umc(pcb, self->umc_socket_descriptor);
			free(pcb);
			sem_post(&sem_cpus_available);
			continue;
		}
		sem_wait(&mutex_cpus_available);
		int cpu  = (int )queue_pop(self->cpus_available);
		sem_post(&mutex_cpus_available);

		pcb->cpu_socket_descriptor = cpu;

		t_pcb_execution_data* data = malloc(sizeof(t_pcb_execution_data));
		data->pcb = pcb;
		data->scheduler = self;
		pthread_t th_pcb_execution;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&th_pcb_execution, &attr, &handle_pcb_execution, (void*) data);
		pthread_attr_destroy(&attr);
	}
}

void* handle_exit(void* scheduler) {
	t_scheduler* self = (t_scheduler*) scheduler;
	while (1) {
		sem_wait(&sem_exit);
		sem_wait(&mutex_exit);
		t_PCB *pcb = queue_pop(self->exit_state);
		sem_post(&mutex_exit);

		int umc_finalizado = end_program_umc(pcb, self->umc_socket_descriptor);
		int consola_finalizado = end_program_console(pcb); //console_finalizado no usarlo para nada es valor 0

		free(pcb);// lo libero directamente creo q no es necesario hacer cola de exit
	}
}

void* handle_io_queue(void* io_attr) {
	t_io_blocked_queue* io_queue = (t_io_blocked_queue*) io_attr;
	t_io* io = io_queue->io;
	while (1) {
		sem_wait(&io_queue->resources);
		t_io_blocked_process_data* blocked_data = queue_pop(io_queue->blocked_pids);
		usleep(io->sleep * blocked_data->operations_count * 1000);
		int same_pid(t_PCB* aux_pcb) {
			return (aux_pcb->pid == blocked_data->pid);
		}
		t_PCB* pcb = list_remove_by_condition(scheduler->block_state, (void*) same_pid);
		free(blocked_data);
		enqueue_to_ready(scheduler, pcb);
	}
}

void enqueue_to_ready(t_scheduler* scheduler, t_PCB* pcb) {
	sem_wait(&mutex_ready);
	pcb->state = "Ready";
	log_trace(kernel_trace,"Ready: %d\n", pcb->pid);
	queue_push(scheduler->ready_state, pcb);
	sem_post(&mutex_ready);
	sem_post(&sem_ready);
}

void enqueue_to_block(t_scheduler* scheduler, t_PCB* pcb) {
	sem_wait(&mutex_block);
	pcb->state = "Bloqueado";
	log_trace(kernel_trace,"Bloqueado: %d\n", pcb->pid);
	list_add(scheduler->block_state, pcb);
	sem_post(&mutex_block);
}

void end_program(t_scheduler* self, t_PCB *pcb) {
    sem_wait(&mutex_exit);
    queue_push(self->exit_state, pcb);
    sem_post(&mutex_exit);
    sem_post(&sem_exit);
}

void free_cpu(t_scheduler* self, int cpu) {
    sem_wait(&mutex_cpus_available);
    log_trace(kernel_trace,"Cpu liberada\n");
    queue_push(self->cpus_available, cpu);
    sem_post(&mutex_cpus_available);
    sem_post(&sem_cpus_available);
}

void wait_block_process(t_scheduler* scheduler, char* sem_id, t_PCB* pcb) {
	int same_sem(t_sem_blocked_queue* sem) {
		if (strcmp(sem->id, sem_id) == 0)
			return 1;
		else
			return 0;
	}
	t_sem_blocked_queue* sem_blocks = list_find(scheduler->semaphores_list, (void*) same_sem);
	queue_push(sem_blocks->blocked_pids, pcb->pid);
	pcb->program_counter ++;
	enqueue_to_block(scheduler, pcb);
	free_cpu(scheduler, pcb->cpu_socket_descriptor);
}

void signal_unblock_process(t_scheduler* scheduler, char* sem_id) {
	int same_sem(t_sem_blocked_queue* sem) {
		if (strcmp(sem->id, sem_id) == 0)
			return 1;
		else
			return 0;
	}
	t_sem_blocked_queue* sem_blocks = list_find(scheduler->semaphores_list, (void*) same_sem);
	int pid = queue_pop(sem_blocks->blocked_pids);
	int same_pid(t_PCB* aux_pcb) {
		return (aux_pcb->pid == pid);
	}
	sem_wait(&mutex_block);
	t_PCB* pcb = list_remove_by_condition(scheduler->block_state, (void*) same_pid);
	sem_post(&mutex_block);
	if (pcb != NULL)
		enqueue_to_ready(scheduler, pcb);
}

void handle_io_operation(t_scheduler* scheduler, char* io_name, int times, t_PCB* pcb) {
	int same_io_queue(t_io_blocked_queue* io_queue) {
		if (strcmp(io_queue->io->name, io_name) == 0)
			return 1;
		else
			return 0;
	}
	t_io_blocked_queue* io_queue = list_find(scheduler->io_list, (void*) same_io_queue);
	t_io_blocked_process_data* blocked_data = malloc(sizeof(t_io_blocked_process_data));
	blocked_data->pid = pcb->pid;
	blocked_data->operations_count = times;
	enqueue_to_block(scheduler, pcb);
	queue_push(io_queue->blocked_pids, blocked_data);
	sem_post(&io_queue->resources);
}

uint32_t check_blocked_pcb(t_scheduler* scheduler, int console_socket_descriptor) {
	int same_descriptor(t_PCB* aux_pcb) {
		return (aux_pcb->console_socket_descriptor == console_socket_descriptor);
	}
	t_PCB* pcb = list_remove_by_condition(scheduler->block_state, (void*) same_descriptor);
	if (pcb != NULL) {
		end_program_umc(pcb, scheduler->umc_socket_descriptor);
		free(pcb);
		return 0;
	} else
		return 1;

}
