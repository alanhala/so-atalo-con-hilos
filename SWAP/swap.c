#include "swap.h"


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
}

void initialize_bitmap(t_swap* self) {
	int page;
	self->bitmap = malloc(self->pages_number * sizeof(int));
	for (page = 0; page < self->pages_number; page ++) {
		*(self->bitmap + page) = 0;
	}
}

t_swap *create_swap() {
	t_swap* self = malloc(sizeof(t_swap));
	t_config* swap_config = config_create("config_file.txt");
	self->port = config_get_int_value(swap_config, "PUERTO_ESCUCHA");
	self->swap_name = config_get_string_value(swap_config, "NOMBRE_SWAP");
	self->pages_number = config_get_int_value(swap_config, "CANTIDAD_PAGINAS");
	self->page_size = config_get_int_value(swap_config, "TAMANIO_PAGINA");
	self->compaction_delay = config_get_int_value(swap_config, "RETARDO_COMPACTACION");
	self->pages_table_list = list_create();
	self->total_pids = 0;
	initialize_bitmap(self);
	create_file(self);
	self->file = fopen(self->swap_name, "r+");

	return self;
}

void destroy_swap(t_swap* self) {
	int i = 0;
	for (i = 0; i < self->total_pids; i ++) {
		destroy_pages_table(list_get(self->pages_table_list, i));
	}
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
	return first_page_location;
}

void add_program_to_bitmap(t_swap* self, unsigned int pages_amount, unsigned int first_page_location) {
	int i;
	for (i = 0; i < pages_amount; i ++) {
		*(self->bitmap + first_page_location) = 1;
		first_page_location ++;
	}
	self->total_pids ++;
}

void write_swap_file(t_swap* self, int first_page_location, unsigned int pages_amount, char * program) {
	int i;
	if (fseek(self->file, first_page_location * self->page_size, SEEK_SET) == 0) {
		fprintf(self->file, program);
	}

	// TODO ver que hacer si fseek falla
}

int initialize_program(t_swap* self, unsigned int pid, unsigned int pages_amount, char* program) {
	int first_page_location = check_space_available(self, pages_amount);
	if (first_page_location != -1) {
		t_pages_table* pages_table = create_pages_table(pid, pages_amount,
				self->page_size, first_page_location);
		add_program_to_bitmap(self, pages_amount, first_page_location);
		write_swap_file(self, first_page_location, pages_amount, program);
		list_add(self->pages_table_list, pages_table);
		return 0;
	}
	else {
		return -1;
	}
}

int write_page(t_swap* self, unsigned int pid, unsigned int page, char* data) {
	t_pages_table* pages_table = find_pages_table(self, pid);
	int page_location = *(pages_table->pages_location + page);
	write_swap_file(self, page_location, 1, data);
	return 0;
}

char* read_page(t_swap* self, unsigned int pid, unsigned int page) {
	char* page_content;
	t_pages_table* pages_table = find_pages_table(self, pid);
	int page_location = *(pages_table->pages_location + page);
	page_content = read_swap_file(self, page_location);
	return page_content;
}

char* read_swap_file(t_swap* self, int page_location) {
	char* page_content = malloc(self->page_size);
	if (fseek(self->file, page_location, SEEK_SET) == 0) {
		printf("%d \n", ftell(self->file));
		int a = fread(page_content, self->page_size, 1, self->file);
		printf("se leyeron %d", a);
	}
	return page_content;
}

t_pages_table* find_pages_table(t_swap* self, unsigned int pid) {
	int same_pid(t_pages_table* pages_table) {
		return (pages_table->pid == pid);
	}

	t_pages_table* pages_table = list_find(self->pages_table_list, (void*) same_pid);
	return pages_table;
}


