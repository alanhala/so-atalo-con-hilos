/*
 * simulacion_de_umc.c
 *
 *  Created on: 1/6/2016
 *      Author: utnso
 */

#include "simulacion_de_umc.h"
#include "CUnit/Basic.h"
#include "../swap.h"
void simulacion_3();

int simulacion_de_umc() {
	CU_initialize_registry();

	CU_pSuite simulacion = CU_add_suite("Simulacion De UMC", NULL, NULL);
	CU_add_test(simulacion, "Simulacion 3", simulacion_3);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}

void simulacion_3(){


	//inicializar_estructuras();
		t_swap* swap = create_swap("config_file.txt");
		char * codigo_pid0= "0pg000pg010pg020pg030pg040pg050pg060pg070pg080pg090pg100pg110pg120pg130pg14";
		char * codigo_pid5 = "5pg005pg015pg025pg035pg045pg055pg065pg075pg085pg095pg105pg115pg125pg135pg14";


		int paginas_necesarias = (30);


		int resultado_carga =initialize_program(swap, 0, paginas_necesarias, codigo_pid0);
		int resultado_carga_pid5 =initialize_program(swap, 5, paginas_necesarias, codigo_pid5);




		int lecturas=0;
		int resultado_lectura = 0;
		for(lecturas; lecturas < 15; lecturas ++){
			char * lo_que_leo =  read_page(swap, 0, lecturas);
			printf("%s\n", lo_que_leo);
		}


		int lecturas_pid5=0;
		int resultado_lectura_pid5 = 0;
		for(lecturas_pid5; lecturas_pid5 < 15; lecturas_pid5 ++){
			char *lo_que_leo_pid5  = read_page(swap, 5, lecturas_pid5);
			printf("%s\n", lo_que_leo_pid5);

		}

		char * lectura_una_pagina =  read_page(swap, 0, 0);
		char * lectura_una_pagina_2 =  read_page(swap, 0, 2);
		char * lectura_una_pagina_10 = read_page(swap, 0, 10);

		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina, "0pg00") , 0);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_2, "0pg02") , 0);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_10, "0pg10") , 0);

		write_page(swap, 0, 12, "piatt");

		char * lectura_una_pagina_12 =  read_page(swap, 0, 12);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_12, "piatt") , 0);

		/*
		escribir_pagina_de_programa(5, 8, 0, TAMANIO_FRAME, "tony");
		escribir_pagina_de_programa(5, 10, 0, TAMANIO_FRAME, "to10");
		escribir_pagina_de_programa(5, 3, 0, TAMANIO_FRAME, "ton3");
		char * lectura_una_pagina_8_pid5 =  leer_pagina_de_programa(5, 8, 0, TAMANIO_FRAME);
		char * lectura_una_pagina_10_pid5 =  leer_pagina_de_programa(5, 10, 0, TAMANIO_FRAME);
		char * lectura_una_pagina_3_pid5 =  leer_pagina_de_programa(5, 3, 0, TAMANIO_FRAME);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_8_pid5, "tony") , 0);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_10_pid5, "to10") , 0);
		CU_ASSERT_EQUAL(strcmp(lectura_una_pagina_3_pid5, "ton3") , 0);
		*/

}
