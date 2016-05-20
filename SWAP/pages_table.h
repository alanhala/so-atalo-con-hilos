#ifndef PAGE_INFO_H
#define PAGE_INFO_H

	#include <stdlib.h>

	typedef struct {
		unsigned int pid;
		int* pages_location;
	} t_pages_table;

	t_pages_table *create_pages_table(unsigned int pid, int pages_number);

	void destroy_pages_table(t_pages_table* self);

	int page_location(t_pages_table* self, int page_number);

#endif
