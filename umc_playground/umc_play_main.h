typedef struct e_t_p{
	int frame;
} t_entrada_tabla_de_paginas;

typedef struct t_p {
	int pid;
	int paginas_totales;
	t_entrada_tabla_de_paginas* entradas;

} t_tablas_de_paginas;

void crear_memoria_principal();
int cargar_archivo_configuracion();
void liberar_memoria_principal();
int inicializar_estructuras();
void cargar_nuevo_programa(int pid, int paginas_requeridas_del_proceso);
void crear_tabla_de_pagina_de_un_proceso(int pid, int paginas_requeridas_del_proceso);
t_entrada_tabla_de_paginas* inicializar_paginas(int paginas_requeridas);
t_tablas_de_paginas* dame_tabla_de_paginas_de_pid(int pid_buscado);
void inicializar_semaforos();
