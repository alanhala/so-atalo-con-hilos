#ifndef SWAP_H
#define SWAP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include "pages_table.h"

int backlog;
char 	*server_ip,
		*umc_port;

	typedef struct {
		FILE* file;
		char* port;
		char* swap_name;
		unsigned int pages_number;
		unsigned int page_size;
		unsigned int compaction_delay;
		t_list* pages_table_list;
		int* bitmap;
		int total_pids;
	} t_swap;

	t_swap *create_swap(char* config_file_path);

	void create_file(t_swap* self);

	void destroy_swap(t_swap* self);

	void add_program(t_swap* self, unsigned int pid, unsigned int pages_number);

	void initialize_bitmap(t_swap* self);

	int check_space_available(t_swap* self, unsigned int pages_amount);

	void add_program_to_bitmap(t_swap* self, unsigned int pages_amount,
			unsigned int first_page_location);

	void write_swap_file(t_swap* self, int first_page_location,
			unsigned int pages_amount, char * program);

	int initialize_program(t_swap* self, unsigned int pid, unsigned int pages_amount, char* program);

	int remove_program(t_swap* self, unsigned int pid);

	char* read_page(t_swap* self, unsigned int pid, unsigned int page);

#endif
