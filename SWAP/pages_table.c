#include "pages_table.h"

t_pages_table *create_pages_table(unsigned int pid, int pages_number) {
	t_pages_table* self = malloc(sizeof(t_pages_table));
	int* pages_location = malloc(pages_number);

	self->pid = pid;
	self->pages_location = pages_location;
	return self;
}

void destroy_pages_table(t_pages_table* self) {
	free(self);
}


int page_location(t_pages_table* self, int page_number) {
	return *(self->pages_location + page_number - 1);
}




