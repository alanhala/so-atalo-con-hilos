/*
 * memoriaPrincipal.h
 *
 *  Created on: 6/5/2016
 *      Author: utnso
 */
//pruebo con extern para ver si corren los test

int TAMANIO_MEMORIA_PRINCIPAL;
char * MEMORIA_PRINCIPAL;
int TAMANIO_FRAME;
int CANTIDAD_FRAMES;

t_list* lista_frames_libres;
t_list* lista_tabla_de_paginas;
sem_t mut_tabla_de_paginas;



typedef struct e_t_p{
	int frame;
} t_entrada_tabla_de_paginas;

typedef struct t_p {
	int pid;
	int paginas_totales;
	t_entrada_tabla_de_paginas* entradas;

} t_tablas_de_paginas;

typedef struct s_f {
	int frame;
	int asignado; //vale 0 si no esta asignado, 1 si esta asignado
} t_frame;


void crear_memoria_principal();
void inicializacion_para_test(int tamanio_frame, int cantidad_frame);
int cargar_archivo_configuracion();
void liberar_memoria_principal();
int inicializar_estructuras();
void cargar_nuevo_programa(int pid, int paginas_requeridas_del_proceso);
void crear_tabla_de_pagina_de_un_proceso(int pid, int paginas_requeridas_del_proceso);
t_entrada_tabla_de_paginas* inicializar_paginas(int paginas_requeridas);
t_tablas_de_paginas* buscar_tabla_de_paginas_de_pid(int pid_buscado);
void inicializar_semaforos();
void asignar_frame_a_una_pagina(t_tablas_de_paginas* tabla, int frame_a_asignar, int pagina);
int devolver_frame_de_pagina(t_tablas_de_paginas* tabla, int pagina);
void escribir_frame_de_memoria_principal(int frame, char* datos);
char* leer_frame_de_memoria_principal(int frame);
void crear_lista_frames_libres();
void agregar_frame_a_lista_de_libres(int numero_de_frame);
void finalizar_programa(int pid); //TODO analizar bien el tema de los frames libres
