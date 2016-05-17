/*
 * tests.c
 *
 *  Created on: 8/5/2016
 *      Author: utnso
 */
#include "CUnit/Basic.h"
#include "tests.h"
#include "nucleo.h"
#include <parser/metadata_program.h>

static const char* example_ansisop_code = "#!/usr/bin/ansisop\nbegin\nvariables a,g\na = 1\ng <­ doble a\nprint g\nend\nfunction doble\nvariables f\nf = $0 + $0\nreturn f\nend\0";

int correrTest(){

	CU_initialize_registry();
	CU_pSuite prueba = CU_add_suite("Suite de prueba", NULL, NULL);
	CU_add_test(prueba, "uno", obtengo_paginas_para_programa);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();

}

void obtengo_paginas_para_programa() {
	t_metadata_program* metadata = metadata_desde_literal(example_ansisop_code);

	CU_ASSERT_EQUAL(obtener_cantidad_paginas_programa(metadata, 8), 11);
}
