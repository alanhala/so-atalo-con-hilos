#include "swap.h"

void create_file(t_swap* self) {
	char* size_of_swap_file = string_itoa(self->pages_number * self->page_size);
	char* incomplete_instruction = "dd if=/dev/zero bs=1 count=";
	char* destiny = " of=";
	char* linux_instruction = malloc(strlen(size_of_swap_file) +
			strlen(incomplete_instruction) + strlen(destiny) + strlen(self->swap_name));

	strcpy(linux_instruction, incomplete_instruction);
	strcat(linux_instruction, size_of_swap_file);
	strcat(linux_instruction, destiny);
	strcat(linux_instruction, self->swap_name);
	system(linux_instruction);
}

void initialize_bitmap(t_swap* self) {
	int page;
	self->bitmap = malloc(self->pages_number);
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
	initialize_bitmap(self);
	create_file(self);
	self->file = fopen(self->swap_name, "r+");

	return self;
}

void destroy_swap(t_swap* self) {
	free(self);
}

void add_program(t_swap* self, unsigned int pid, unsigned int pages_number) {
	t_pages_table* pages_table = create_pages_table(pid, pages_number);
	list_add(self->pages_table_list, pages_table);
}
