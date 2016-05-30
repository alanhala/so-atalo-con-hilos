#include "pages_table.h"

t_pages_table *create_pages_table(unsigned int pid, int pages_number,
		unsigned int page_size, int first_page_location) {
	t_pages_table* self = malloc(sizeof(t_pages_table));
	int* pages_location = malloc(sizeof(int) * pages_number);
	int current_page = 0;
	for(current_page = 0; current_page < pages_number; current_page ++) {
		*(pages_location + current_page) = (first_page_location + current_page) * page_size;
	}
	self->pid = pid;
	self->pages_location = pages_location;
	self->pages_number = pages_number;
	return self;
}

void destroy_pages_table(t_pages_table* self) {
	free(self->pages_location);
	free(self);
}


int page_location(t_pages_table* self, int page_number) {
	return *(self->pages_location + page_number - 1);
}




