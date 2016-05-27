#include "swap_spec.h"

int test() {
	CU_initialize_registry();

	CU_pSuite swap_creation = CU_add_suite("Swap Creation Test", NULL, NULL);
	CU_add_test(swap_creation, "loads configuration file", loads_configuration_file);
	CU_add_test(swap_creation, "creates the file with the right size", creates_file);
	CU_add_test(swap_creation, "initializes the bitmap", initializes_the_bitmap);


	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

void loads_configuration_file() {
	t_swap* swap = create_swap();
	CU_ASSERT_EQUAL(swap->port, 6000);
	CU_ASSERT_EQUAL(swap->pages_number, 5);
	CU_ASSERT_EQUAL(swap->page_size, 1024);
	CU_ASSERT_EQUAL(swap->compaction_delay, 6000);
}

void creates_file() {
	t_swap* swap = create_swap();
	fseek(swap->file, 0, SEEK_END);
	CU_ASSERT_EQUAL(ftell(swap->file), swap->pages_number * swap->page_size);
}

void initializes_the_bitmap() {
	t_swap* swap = create_swap();
	int i;
	int pages_initialized = 0;
	for (i = 0; i < swap->pages_number; i++) {
		if (*(swap->bitmap + i) == 0) {
			pages_initialized ++;
		}
	}

	CU_ASSERT_EQUAL(pages_initialized, swap->pages_number);
}
