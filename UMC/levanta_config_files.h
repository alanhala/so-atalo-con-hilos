/*
 * levanta_config_files.h
 *
 *  Created on: 20/6/2016
 *      Author: utnso
 */

#ifndef LEVANTA_CONFIG_FILES_H_
#define LEVANTA_CONFIG_FILES_H_

#include <commons/log.h>
#include <commons/config.h>
#include <commons/error.h>
#include "main.h"
#include <sys/inotify.h>
#include <stdlib.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof (struct inotify_event) + 24)
#define BUF_LEN (1024 * EVENT_SIZE)

typedef struct umcConfigFile {
	unsigned marcos,
			marcos_size,
			marco_x_proc,
			entradas_tlb,
			retardo;
	int		backlog;
	char 	*ip_swap,
			*algoritmo_reemplazo,
			*puerto_swap,
			*ip_server,
			*puerto;
} UMCConfigFile;

void levantaConfigFileEnVariables(UMCConfigFile *ptrvaloresConfigFile,t_config *ptrConfig);
int *cargar_configuracion();
char* leer_string(t_config *config, char* key);
unsigned leerUnsigned(t_config *config, char* key);
void liberaVariables(t_log* trace_log, t_config* ptrConfig, t_config* ptrConfigUpdate);
void detectaCambiosEnConfigFile();

#endif /* LEVANTA_CONFIG_FILES_H_ */
