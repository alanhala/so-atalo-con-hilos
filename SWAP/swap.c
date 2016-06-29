#include "swap.h"

/*
Para usar Logs
extern t_log *trace_log_SWAP;
log_trace(trace_log_SWAP,"<lo_que_quieran_loggear>");
*/

carga_variables_globales_swap(t_config *swap_config);

void create_file(t_swap* self) {
	char* size_of_swap_file = string_itoa(self->pages_number * self->page_size);
	char* incomplete_instruction = "dd if=/dev/zero bs=1 count=";
	char* destiny = " of=";
	char* linux_instruction = malloc(strlen(size_of_swap_file) +
			strlen(incomplete_instruction) + strlen(destiny) + strlen(self->swap_name) + 3);

	strcpy(linux_instruction, incomplete_instruction);
	strcat(linux_instruction, size_of_swap_file);
	strcat(linux_instruction, destiny);
	strcat(linux_instruction, self->swap_name);
	system(linux_instruction);
	free(linux_instruction);

	extern t_log *trace_log_SWAP;
	log_trace(trace_log_SWAP,"---------\n");
}

void initialize_bitmap(t_swap* self) {
	int page;
	self->bitmap = malloc(self->pages_number * sizeof(int));
	for (page = 0; page < self->pages_number; page ++) {
		*(self->bitmap + page) = 0;
	}
}

t_swap *create_swap(char* config_file_path) {
	t_swap* self = malloc(sizeof(t_swap));
	t_config* swap_config = config_create(config_file_path);
	umc_port = self->port = config_get_string_value(swap_config, "PUERTO_ESCUCHA");
	self->swap_name = config_get_string_value(swap_config, "NOMBRE_SWAP");
	self->pages_number = config_get_int_value(swap_config, "CANTIDAD_PAGINAS");
	self->page_size = config_get_int_value(swap_config, "TAMANIO_PAGINA");
	self->compaction_delay = config_get_int_value(swap_config, "RETARDO_COMPACTACION");
	self->pages_table_list = list_create();
	initialize_bitmap(self);
	create_file(self);
	self->file = fopen(self->swap_name, "r+");

	server_ip = config_get_string_value(swap_config,"SERVER_IP");

	return self;
}

void destroy_swap(t_swap* self) {
	int i = 0;
	for (i = 0; i < self->pages_table_list->elements_count; i ++) {
		destroy_pages_table(list_get(self->pages_table_list, i));
	}
	free(self->bitmap);
	fclose(self->file);
	list_destroy(self->pages_table_list);
	free(self->swap_name);
	free(self);
}

int check_space_available(t_swap* self, unsigned int pages_amount) {
	int first_page_location = -1;
	int i, free_slots = 0;
	for (i = 0; i < self->pages_number; i++) {
		if (*(self->bitmap + i) == 0) {
			free_slots ++;
			if (first_page_location == -1)
				first_page_location = i;
			if (free_slots == pages_amount)
				break;
		}
		else {
			first_page_location = -1;
			free_slots = 0;
		}
	}
	if (free_slots < pages_amount)
		first_page_location = -1;
	return first_page_location;
}

void add_program_to_bitmap(t_swap* self, unsigned int pages_amount, unsigned int first_page_location) {
	int i;
	for (i = 0; i < pages_amount; i ++) {
		*(self->bitmap + first_page_location) = 1;
		first_page_location ++;
	}
}

void write_swap_file(t_swap* self, int first_page_location, unsigned int pages_amount, char * program) {
	int i;
	if (fseek(self->file, first_page_location, SEEK_SET) == 0) {
		fwrite(program, strlen(program), 1, self->file);
		int not_written_bytes = pages_amount * self->page_size - strlen(program);
		if (not_written_bytes > 0) {
			for (i = 0; i < not_written_bytes; i++)
				fwrite("0", 1, 1, self->file);
		}
	}

	// TODO ver que hacer si fseek falla
}

int initialize_program(t_swap* self, unsigned int pid, unsigned int pages_amount, char* program) {
	int first_page = check_space_available(self, pages_amount);
	if (first_page != -1) {
		t_pages_table* pages_table = create_pages_table(pid, pages_amount,
				self->page_size, first_page);
		add_program_to_bitmap(self, pages_amount, first_page);
		write_swap_file(self, first_page * self->page_size, pages_amount, program);
		list_add(self->pages_table_list, pages_table);
		return 0;
	}
	else {
		return -1;
	}
}

t_pages_table* find_pages_table(t_swap* self, unsigned int pid) {
	int same_pid(t_pages_table* pages_table) {
		return (pages_table->pid == pid);
	}
	t_pages_table* pages_table = list_find(self->pages_table_list, (void*) same_pid);
	return pages_table;
}

int write_page(t_swap* self, unsigned int pid, unsigned int page, char* data) {
	t_pages_table* pages_table = find_pages_table(self, pid);
	int page_location = *(pages_table->pages_location + page);
	write_swap_file(self, page_location, 1, data);
	return 0;
}

char* read_swap_file(t_swap* self, int page_location) {
	char* page_content = malloc(self->page_size);
	if (fseek(self->file, page_location, SEEK_SET) == 0) {
		fread(page_content, self->page_size, 1, self->file);
	}
	return page_content;
}

char* read_page(t_swap* self, unsigned int pid, unsigned int page) {
	char* page_content;
	t_pages_table* pages_table = find_pages_table(self, pid);
	int page_location = *(pages_table->pages_location + page);
	page_content = read_swap_file(self, page_location);
	return page_content;
}

void remove_program_from_bitmap(t_swap* self, unsigned int first_page, unsigned int pages_number) {
	int i;
	for (i = 0; i < pages_number; i ++)
		*(self->bitmap + first_page + i) = 0;
}

t_pages_table* remove_pages_table(t_swap* self, unsigned int pid) {
	int same_pid(t_pages_table* pages_table) {
		return (pages_table->pid == pid);
	}
	return list_remove_by_condition(self->pages_table_list, (void*) same_pid);
}

int remove_program(t_swap* self, unsigned int pid) {
	t_pages_table* pages_table = remove_pages_table(self, pid);
	int first_page = *(pages_table->pages_location) / self->page_size;
	remove_program_from_bitmap(self, first_page, pages_table->pages_number);
	destroy_pages_table(pages_table);
	return 0;
}

