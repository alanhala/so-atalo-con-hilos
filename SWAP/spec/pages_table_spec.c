#include "pages_table_spec.h"

int pages_table_spec() {
	CU_initialize_registry();

	CU_pSuite pages_table_creation = CU_add_suite("Pages table creation", NULL, NULL);
	CU_add_test(pages_table_creation, "assigns the right pid",
			assigns_rigth_pid);
	CU_add_test(pages_table_creation, "assigns the right number of pages",
			assigns_right_pages);
	CU_add_test(pages_table_creation, "saves the rigth pages locations",
			assigns_rigth_location);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

void assigns_rigth_pid() {
	t_pages_table* pages_table = create_pages_table(2, 4, 3, 3);
	CU_ASSERT_EQUAL(pages_table->pid, 2);
}

void assigns_right_pages() {
	t_pages_table* pages_table = create_pages_table(2, 4, 3, 3);
	CU_ASSERT_EQUAL(pages_table->pages_number, 4);
}

void assigns_rigth_location() {
	t_pages_table* pages_table = create_pages_table(2, 4, 3, 3);
	CU_ASSERT_EQUAL(*(pages_table->pages_location), 9);
	CU_ASSERT_EQUAL(*(pages_table->pages_location + 1), 12);
	CU_ASSERT_EQUAL(*(pages_table->pages_location + 2), 15);
	CU_ASSERT_EQUAL(*(pages_table->pages_location + 3), 18);
}
