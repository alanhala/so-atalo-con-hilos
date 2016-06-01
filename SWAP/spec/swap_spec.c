#include "swap_spec.h"

int swap_spec() {
	CU_initialize_registry();

	CU_pSuite swap_creation = CU_add_suite("Swap Creation Test", NULL, NULL);
	CU_add_test(swap_creation, "loads configuration file", loads_configuration_file);
	CU_add_test(swap_creation, "creates the file with the right size", creates_file);
	CU_add_test(swap_creation, "initializes the bitmap", initializes_the_bitmap);

	CU_pSuite check_space_available = CU_add_suite("Check space available for new program", NULL, NULL);
	CU_add_test(check_space_available, "when bitmap is empty, it loads the program in the first location",
			check_space_available_1);
	CU_add_test(check_space_available, "when bitmap has already a program and it has space for another next to it, it returns the next location",
			check_space_available_2);
	CU_add_test(check_space_available, "when bitmap has already a program and it does not have space for another next to it, it returns -1",
			check_space_available_3);
	CU_add_test(check_space_available, "when bitmap has two programs with space between them, and a new program does not fit between them, it returns the next location after the second program",
			check_space_available_4);
	CU_add_test(check_space_available, "when bitmap has two programs with space between them, and a new program fits there, it returns the next location after the first program",
			check_space_available_5);
	CU_add_test(check_space_available, "when bitmap has two programs with space between them, and a new program does not fit anywhere, it returns -1",
			check_space_available_6);

	CU_pSuite add_program_to_bitmap = CU_add_suite("Add a new program to the bitmap", NULL, NULL);
	CU_add_test(add_program_to_bitmap, "it adds the new program right after the specific page",
			add_program_to_bitmap_1);
	CU_add_test(add_program_to_bitmap, "it does not modify the other pages",
			add_program_to_bitmap_2);

	CU_pSuite write_swap_file = CU_add_suite("Write swap file", NULL, NULL);
	CU_add_test(write_swap_file, "having 3 pages of a program, it writes after the specified byte",
			write_swap_file_1);
	CU_add_test(write_swap_file, "having an empty file, it writes after the first byte",
			write_swap_file_2);
	CU_add_test(write_swap_file, "having a page already written, it overwrites it with the new one",
			write_swap_file_3);
	CU_add_test(write_swap_file, "having a page already written and the content of the new page is smaller, it fills the page with \0",
			write_swap_file_4);

	CU_pSuite read_page = CU_add_suite("read swap file", NULL, NULL);
	CU_add_test(read_page, "it reads a page from an existing program",
			read_page_1);
	CU_add_test(read_page, "it reads a not completed page",
			read_page_2);
	CU_add_test(read_page, "having two programs on the swap, it reads the rigth one",
			read_page_2);

	CU_pSuite initialize_program = CU_add_suite("initialize new program", NULL, NULL);
	CU_add_test(initialize_program, "it creates a new pages table for that program and it adds it to the swap table pages list",
			initialize_program_1);
	CU_add_test(initialize_program, "it returns 0 if there's space for that program",
			initialize_program_2);
	CU_add_test(initialize_program, "it returns -1 if there's no space for that program",
			initialize_program_3);
	CU_add_test(initialize_program, "adds the program to the bitmap",
			initialize_program_4);
	CU_add_test(initialize_program, "writes the swap file",
			initialize_program_5);

	CU_pSuite remove_program = CU_add_suite("remove an existing program", NULL, NULL);
	CU_add_test(remove_program, "marks the bitmap as empty",
			remove_program_1);
	CU_add_test(remove_program, "remove the pages table",
			remove_program_2);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

void loads_configuration_file() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	CU_ASSERT_NSTRING_EQUAL(swap->port, "6000", 4);
	CU_ASSERT_EQUAL(swap->pages_number, 10);
	CU_ASSERT_EQUAL(swap->page_size, 5);
	CU_ASSERT_EQUAL(swap->compaction_delay, 6000);
}

void creates_file() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	fseek(swap->file, 0, SEEK_END);
	CU_ASSERT_EQUAL(ftell(swap->file), swap->pages_number * swap->page_size);
}

void initializes_the_bitmap() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	int i;
	int pages_initialized = 0;
	for (i = 0; i < swap->pages_number; i++) {
		if (*(swap->bitmap + i) == 0) {
			pages_initialized ++;
		}
	}

	CU_ASSERT_EQUAL(pages_initialized, swap->pages_number);
}

void check_space_available_1() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	int first_page_location = check_space_available(swap, 4);
	CU_ASSERT_EQUAL(first_page_location, 0);
}

void check_space_available_2() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	add_program_to_bitmap(swap, 4, 0);
	int first_page_location = check_space_available(swap, 5);
	CU_ASSERT_EQUAL(first_page_location, 4);
}

void check_space_available_3() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	add_program_to_bitmap(swap, 4, 0);
	int first_page_location = check_space_available(swap, 7);
	CU_ASSERT_EQUAL(first_page_location, -1);
}

void check_space_available_4() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	add_program_to_bitmap(swap, 2, 0);
	add_program_to_bitmap(swap, 2, 4);
	int first_page_location = check_space_available(swap, 4);
	CU_ASSERT_EQUAL(first_page_location, 6);
}

void check_space_available_5() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	add_program_to_bitmap(swap, 2, 0);
	add_program_to_bitmap(swap, 2, 4);
	int first_page_location = check_space_available(swap, 2);
	CU_ASSERT_EQUAL(first_page_location, 2);
}

void check_space_available_6() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	add_program_to_bitmap(swap, 2, 0);
	add_program_to_bitmap(swap, 2, 4);
	int first_page_location = check_space_available(swap, 7);
	CU_ASSERT_EQUAL(first_page_location, -1);
}

void add_program_to_bitmap_1() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	add_program_to_bitmap(swap, 4, 3);
	int i;
	int pages_saved = 0;
	for (i = 3; i <= 6; i++) {
		if (*(swap->bitmap + i) == 1) {
			pages_saved ++;
		}
	}
	CU_ASSERT_EQUAL(pages_saved, 4);
}

void add_program_to_bitmap_2() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	add_program_to_bitmap(swap, 4, 3);
	int i;
	int pages_not_modified = 0;
	for (i = 0; i < 3; i++) {
		if (*(swap->bitmap + i) == 0) {
			pages_not_modified ++;
		}
	}
	for (i = 7; i < 10; i++) {
		if (*(swap->bitmap + i) == 0) {
			pages_not_modified ++;
		}
	}
	CU_ASSERT_EQUAL(pages_not_modified, 6);
}

void write_swap_file_1() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	write_swap_file(swap, 0, 1, "12345");
	write_swap_file(swap, 3, 3, "qwertyuiopasdfg");
	char* first_char = malloc(15);
	fseek(swap->file, 15, SEEK_SET);
	fread(first_char, 15, 1, swap->file);
	CU_ASSERT_NSTRING_EQUAL(first_char, "qwertyuiopasdfg", 15);
}

void write_swap_file_2() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	write_swap_file(swap, 0, 1, "12345");
	char* data = malloc(5);
	fseek(swap->file, 0, SEEK_SET);
	fread(data, 15, 1, swap->file);
	CU_ASSERT_NSTRING_EQUAL(data, "12345", 5);
}

void write_swap_file_3() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	write_swap_file(swap, 0, 1, "12345");
	write_swap_file(swap, 0, 1, "qwert");
	char* data = malloc(5);
	fseek(swap->file, 0, SEEK_SET);
	fread(data, 5, 1, swap->file);
	CU_ASSERT_NSTRING_EQUAL(data, "qwert", 5);
}

void write_swap_file_4() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	write_swap_file(swap, 0, 1, "12345");
	write_swap_file(swap, 0, 1, "qwe");
	char* data = malloc(5);
	fseek(swap->file, 0, SEEK_SET);
	fread(data, 5, 1, swap->file);
	CU_ASSERT_NSTRING_EQUAL(data, "qwe", 5);
}

void read_page_1() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	initialize_program(swap, 1, 1, "12345");
	char* data = malloc(5);
	data = read_page(swap, 1, 0);
	CU_ASSERT_NSTRING_EQUAL(data, "12345", 5);
}

void read_page_2() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	initialize_program(swap, 1, 1, "123");
	char* data = malloc(5);
	data = read_page(swap, 1, 0);
	CU_ASSERT_NSTRING_EQUAL(data, "123", 5);
}

void read_page_3() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	initialize_program(swap, 1, 1, "123");
	initialize_program(swap, 2, 2, "1234567890");
	char* data = malloc(5);
	data = read_page(swap, 2, 1);
	CU_ASSERT_NSTRING_EQUAL(data, "67890", 5);
}

void initialize_program_1() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	initialize_program(swap, 1, 3, "123456789012345");
	t_pages_table* pages_table = find_pages_table(swap, 1);
	CU_ASSERT_EQUAL(swap->pages_table_list->elements_count, 1);
	CU_ASSERT_EQUAL(pages_table->pid, 1);
	CU_ASSERT_EQUAL(pages_table->pages_number, 3);
	CU_ASSERT_EQUAL(*(pages_table->pages_location), 0);
	CU_ASSERT_EQUAL(*(pages_table->pages_location + 1), 5);
	CU_ASSERT_EQUAL(*(pages_table->pages_location + 2), 10);
}

void initialize_program_2() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	int result = initialize_program(swap, 1, 3, "123456789012345");
	CU_ASSERT_EQUAL(result, 0);
}

void initialize_program_3() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	int result = initialize_program(swap, 1, 11, "123456789012345");
	CU_ASSERT_EQUAL(result, -1);
}

void initialize_program_4() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	initialize_program(swap, 1, 2, "1234567890");
	CU_ASSERT_EQUAL(*(swap->bitmap), 1);
	CU_ASSERT_EQUAL(*(swap->bitmap + 1), 1);
	CU_ASSERT_EQUAL(*(swap->bitmap + 2), 0);
}

void initialize_program_5() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	initialize_program(swap, 1, 2, "1234567890");
	char* data = malloc(5);
	data = read_page(swap, 1, 1);
	CU_ASSERT_NSTRING_EQUAL(data, "67890", 5);
}

void remove_program_1() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	initialize_program(swap, 1, 2, "1234567890");
	initialize_program(swap, 2, 3, "123456789012345");
	remove_program(swap, 2);
	CU_ASSERT_EQUAL(*(swap->bitmap), 1);
	CU_ASSERT_EQUAL(*(swap->bitmap + 1), 1);
	CU_ASSERT_EQUAL(*(swap->bitmap + 2), 0);
	CU_ASSERT_EQUAL(*(swap->bitmap + 3), 0);
	CU_ASSERT_EQUAL(*(swap->bitmap + 4), 0);
}

void remove_program_2() {
	t_swap* swap = create_swap("./spec/config_file_test.txt");
	initialize_program(swap, 1, 2, "1234567890");
	initialize_program(swap, 2, 3, "123456789012345");
	remove_program(swap, 2);
	CU_ASSERT_EQUAL(swap->pages_table_list->elements_count, 1);
}


