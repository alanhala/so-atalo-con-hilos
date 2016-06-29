#include "kernel_spec.h"

int kernel_spec() {
	CU_initialize_registry();

	CU_pSuite create_kernel = CU_add_suite("Kernel creation", NULL, NULL);
	CU_add_test(create_kernel, "it creates the kernel with the right attributes",
			create_kernel_1);
	CU_add_test(create_kernel, "it creates the right io_list",
			create_kernel_2);
	CU_add_test(create_kernel, "it creates the right shared variables list",
			create_kernel_3);
	CU_add_test(create_kernel, "it initialize the semaphores",
			create_kernel_4);

	CU_pSuite initialize_program = CU_add_suite("Initialize program", NULL, NULL);
	CU_add_test(initialize_program, "it increments the programs number",
			initialize_program_1);
	CU_add_test(initialize_program, "it creates the right pcb",
			initialize_program_2);
	CU_add_test(initialize_program, "having a program with a label, it assigns the right label index",
			initialize_program_3);
	CU_add_test(initialize_program, "having a program with more than one label, it assigns the right label indexes",
			initialize_program_4);

	CU_pSuite get_shared_variable_value = CU_add_suite("get shared variable value", NULL, NULL);
	CU_add_test(get_shared_variable_value, "it returns the right value",
			get_shared_var_value_1);

	CU_pSuite update_shared_variable_value = CU_add_suite("update shared variable value", NULL, NULL);
	CU_add_test(update_shared_variable_value, "it updates the variable with the right value",
			update_shared_var_value_1);

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

void create_kernel_2() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	t_io* io1 = list_get(kernel->io_list, 0);
	t_io* io2 = list_get(kernel->io_list, 1);
	t_io* io3 = list_get(kernel->io_list, 2);
	CU_ASSERT_EQUAL(kernel->io_list->elements_count, 3);
	CU_ASSERT_STRING_EQUAL(io1->name, "Disco");
	CU_ASSERT_EQUAL(io1->sleep, 1000);
	CU_ASSERT_STRING_EQUAL(io2->name, "Impresora");
	CU_ASSERT_EQUAL(io2->sleep, 2000);
	CU_ASSERT_STRING_EQUAL(io3->name, "Scanner");
	CU_ASSERT_EQUAL(io3->sleep, 1000);
}

void create_kernel_3() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	t_shared_variable* var_1 = list_get(kernel->shared_vars, 0);
	t_shared_variable* var_2 = list_get(kernel->shared_vars, 1);
	t_shared_variable* var_3 = list_get(kernel->shared_vars, 2);
	CU_ASSERT_EQUAL(kernel->shared_vars->elements_count, 3);
	CU_ASSERT_STRING_EQUAL(var_1->name, "!Global");
	CU_ASSERT_EQUAL(var_1->value, 0);
	CU_ASSERT_STRING_EQUAL(var_2->name, "!UnaVar");
	CU_ASSERT_EQUAL(var_2->value, 0);
	CU_ASSERT_STRING_EQUAL(var_3->name, "!tiempo3");
	CU_ASSERT_EQUAL(var_3->value, 0);
}

void create_kernel_4() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	t_semaphore* sem_1 = list_get(kernel->semaphores, 0);
	t_semaphore* sem_2 = list_get(kernel->semaphores, 1);
	t_semaphore* sem_3 = list_get(kernel->semaphores, 2);
	CU_ASSERT_STRING_EQUAL(sem_1->id, "SEM1");
	CU_ASSERT_STRING_EQUAL(sem_2->id, "SEM2");
	CU_ASSERT_STRING_EQUAL(sem_3->id, "SEM3");
	CU_ASSERT_EQUAL(sem_1->value, 0);
	CU_ASSERT_EQUAL(sem_2->value, 0);
	CU_ASSERT_EQUAL(sem_3->value, 5);
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

void get_shared_var_value_1() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	t_shared_variable* var = list_get(kernel->shared_vars, 2);
	var->value = 42;
	int var_value = get_shared_var_value(kernel, "!tiempo3");
	CU_ASSERT_EQUAL(var_value, 42);
}

void update_shared_var_value_1() {
	t_kernel* kernel = create_kernel("./spec/kernel_config_spec.txt");
	update_shared_var_value(kernel, "!UnaVar", 25);
	CU_ASSERT_EQUAL(get_shared_var_value(kernel, "!UnaVar"), 25)
}


