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

int ALGORITMO_REEMPLAZO;


int	CANTIDAD_ENTRADAS_TLB;
int TLB_HABILITADA;
int MAX_FRAMES_POR_PROCESO;
int RETARDO; //no hay varios retardos? uno para tlb, otro para mp? la del swap depende de swap...

t_list* lista_frames;
sem_t mut_lista_frames;
t_list* lista_tabla_de_paginas;
sem_t mut_tabla_de_paginas;



typedef struct e_t_p{
	int frame; // si no tiene frame asignado setear en -1
	int utilizado;
	int modificado;
} t_entrada_tabla_de_paginas;



typedef struct t_p {
	int pid;
	int paginas_totales;
	t_entrada_tabla_de_paginas* entradas;
	int frames_en_uso;

} t_tabla_de_paginas;

typedef struct s_f {
	int frame;
	int asignado; //vale 0 si no esta asignado, 1 si esta asignado
} t_frame;


typedef struct entradatlb {
	int pid;
	int pag;
	int frame;
} entrada_tlb;

typedef struct tlb {
	entrada_tlb* entradas;
} tabla_tlb;


void escribir_frame_de_memoria_principal(int frame, int offset, int size, char* datos);
char* leer_pagina_de_programa(t_tabla_de_paginas * tabla, int pagina, int offset, int size);
int escribir_pagina_de_programa(t_tabla_de_paginas * tabla, int pagina, int offset, int size, char * buffer);

void actualizar_frame(t_tabla_de_paginas * tabla, int frame);
void crear_memoria_principal();
void inicializacion_para_test(int tamanio_frame, int cantidad_frame);
int cargar_archivo_configuracion();
void liberar_memoria_principal();
int inicializar_estructuras();
int cargar_nuevo_programa(int pid, int paginas_requeridas_del_proceso, char * codigo_programa);
t_tabla_de_paginas * crear_tabla_de_pagina_de_un_proceso(int pid, int paginas_requeridas_del_proceso);
t_entrada_tabla_de_paginas* inicializar_paginas(int paginas_requeridas);
t_tabla_de_paginas* buscar_tabla_de_paginas_de_pid(int pid_buscado);
void inicializar_semaforos();
void asignar_frame_a_una_pagina(t_tabla_de_paginas* tabla, int frame_a_asignar, int pagina);
int devolver_frame_de_pagina(t_tabla_de_paginas* tabla, int pagina);
char* leer_frame_de_memoria_principal(int frame, int offset, int size);
void crear_lista_frames();
void agregar_frame_a_lista_de_frames(int numero_de_frame);
void finalizar_programa(int pid);
void marcar_frame_como_libre(int numero_de_frame);
int buscar_frame_libre();
int buscar_frame_de_una_pagina(t_tabla_de_paginas* tabla, int pagina);
int tiene_tabla_mas_paginas_para_pedir(t_tabla_de_paginas* tabla);
int darle_frame_a_una_pagina(t_tabla_de_paginas* tabla, int pagina);
int buscar_en_tlb_frame_de_pagina(int tabla, int pagina);
void pedir_a_swap_la_pagina_y_actualizar_memoria_principal(int pid, int pagina, int frame_de_pagina);


int cargar_nuevo_programa_en_swap(int pid, int paginas_requeridas_del_proceso, char *codigo_programa);

// TLB
tabla_tlb* crear_tlb();



// CONFIGURACION

void set_cantidad_entradas_tlb(int entradas);
void set_max_frames_por_proceso(int cantidad);
void set_cantidad_frames(int cantidad_marcos);
void set_tamanio_frame(int tamanio_marcos);
void set_retardo(int retardo);
void set_algoritmo_reemplazo(char * algoritmo);

