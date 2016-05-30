#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

	#include <stdlib.h>

	typedef struct {
		unsigned int pid;
		unsigned int pages_number;
		int* pages_location;
	} t_pages_table;

	t_pages_table *create_pages_table(unsigned int pid, int pages_number,
			unsigned int page_size, int first_page_location);

	void destroy_pages_table(t_pages_table* self);

	int page_location(t_pages_table* self, int page_number);

#endif
