#ifndef SWAP_H
#define SWAP_H

	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <commons/config.h>
	#include <commons/string.h>
	#include <commons/collections/list.h>
	#include "pages_table.h"

	typedef struct {
		FILE* file;
		int port;
		char* swap_name;
		unsigned int pages_number;
		unsigned int page_size;
		unsigned int compaction_delay;
		t_list* pages_table_list;
		int* bitmap;
	} t_swap;

	t_swap *create_swap();

	void create_file(t_swap* self);

	void destroy_swap(t_swap* self);

	void add_program(t_swap* self, unsigned int pid, unsigned int pages_number);

	void initialize_bitmap(t_swap* self);

#endif
