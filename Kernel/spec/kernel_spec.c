#include "kernel_spec.h"

int kernel_spec() {
	CU_initialize_registry();

	CU_pSuite create_kernel = CU_add_suite("Kernel creation", NULL, NULL);
	CU_add_test(create_kernel, "it creates the kernel with the right attributes",
			create_kernel_1);

	CU_pSuite initialize_program = CU_add_suite("Initialize program", NULL, NULL);
	CU_add_test(initialize_program, "it increments the programs number",
			initialize_program_1);
	CU_add_test(initialize_program, "it creates the right pcb",
			initialize_program_2);
	CU_add_test(initialize_program, "having a program with a label, it assigns the right label index",
			initialize_program_3);
	CU_add_test(initialize_program, "having a program with more than one label, it assigns the right label indexes",
			initialize_program_4);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

void create_kernel_1() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	CU_ASSERT_EQUAL(kernel->programs_number, 0);
	CU_ASSERT_STRING_EQUAL(kernel->console_port, "5000");
	CU_ASSERT_STRING_EQUAL(kernel->cpu_port, "5001");
	CU_ASSERT_EQUAL(kernel->quantum, 3);
	CU_ASSERT_EQUAL(kernel->quantum_sleep, 500);
	CU_ASSERT_EQUAL(kernel->stack_size, 2);
}

void initialize_program_1() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	initialize_program(kernel, "begin\nvariables c, d\nc=1234\nd=4321\nend\0");
	CU_ASSERT_EQUAL(kernel->programs_number, 1);
}

void initialize_program_2() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	t_PCB* pcb = initialize_program(kernel, "begin\nvariables c, d\nc=1234\nd=4321\nend");
	CU_ASSERT_EQUAL(pcb->pid, 0);
	CU_ASSERT_EQUAL(pcb->instructions_size, 4);
	CU_ASSERT_EQUAL(pcb->instructions_index->start, 6);
	CU_ASSERT_EQUAL(pcb->instructions_index->offset, 15);
	CU_ASSERT_EQUAL((pcb->instructions_index)[1].start, 21);
	CU_ASSERT_EQUAL((pcb->instructions_index)[1].offset, 7);
	CU_ASSERT_EQUAL((pcb->instructions_index)[2].start, 28);
	CU_ASSERT_EQUAL((pcb->instructions_index)[2].offset, 7);
	CU_ASSERT_EQUAL((pcb->instructions_index)[3].start, 35);
	CU_ASSERT_EQUAL((pcb->instructions_index)[3].offset, 4);
	CU_ASSERT_EQUAL(pcb->program_counter, 0);
	CU_ASSERT_EQUAL(pcb->stack_last_address->page, 7);
	CU_ASSERT_EQUAL(pcb->stack_last_address->offset, 3);
}

void initialize_program_3() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	t_PCB* pcb = initialize_program(kernel, "begin\nvariables i,b\ni = 1\n:inicio_for\ni = i + 1\nprint i\nb = i - 10\njnz b inicio_for\n#fuera del for\nend");
	t_label_index* label_index = list_get(pcb->label_index, 0);
	CU_ASSERT_STRING_EQUAL(label_index->name, "inicio_for");
	CU_ASSERT_EQUAL(label_index->location, 2);
}

void initialize_program_4() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	t_PCB* pcb = initialize_program(kernel, "begin\nf\nend\n\nfunction f\nvariables a\na=1\nprint a\ng\nend\n\nfunction g\nvariables a\na=0\nprint a\nf\nend");
	t_label_index* label_index1 = list_get(pcb->label_index, 0);
	t_label_index* label_index2 = list_get(pcb->label_index, 1);
	CU_ASSERT_STRING_EQUAL(label_index1->name, "f");
	CU_ASSERT_EQUAL(label_index1->location, 2);
	CU_ASSERT_STRING_EQUAL(label_index2->name, "g");
	CU_ASSERT_EQUAL(label_index2->location, 7);
}



