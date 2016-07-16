/*
 * levanta_config_file.c
 *
 *  Created on: 25/6/2016
 *      Author: utnso
 */

#include "levanta_config_file.h"
#include <commons/config.h>
#include <commons/log.h>
#include <commons/error.h>
#include <sys/inotify.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"

int huboUnCambio;

void *cargar_configuracion(t_kernel *kernel){

	t_log *trace_log_Config_Files;
	trace_log_Config_Files = log_create("./Log_de_Config_Files.txt","main.c",false,LOG_LEVEL_TRACE);

	t_config *ptrConfig, *ptrConfigUpdate;

	KernelConfigFile *ptrvaloresConfigFile = malloc(sizeof(KernelConfigFile));

	ptrConfig = config_create("./kernel_config.txt");
	if (ptrConfig == NULL){
		log_trace(trace_log_Config_Files,"Archivo de configuración no disponible. No puede ejecutar el Kernel.\n");
		return (1);
	}

	log_trace(trace_log_Config_Files,"Cargando parametros del Archivo de Configuracion\n");

	levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfig);
	cargar_kernel(ptrvaloresConfigFile,kernel);
	carga_variables_globales(ptrvaloresConfigFile);
	log_trace(trace_log_Config_Files,"Archivo de Configuracion levantado exitosamente.\n");
	sem_post(&sem_config_file_kernel);

	while(1)
	{
		huboUnCambio = 0;
		detectaCambiosEnConfigFile();
		if(huboUnCambio)
		{
			ptrConfigUpdate = config_create("./kernel_config.txt");
			if(ptrConfigUpdate->properties->elements_amount==0) {
				log_trace(trace_log_Config_Files,"No se puede levantar el Archivo de Configuracion\n");
			} else {
				levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfigUpdate);
				cargar_kernel(ptrvaloresConfigFile,kernel);
				carga_variables_globales(ptrvaloresConfigFile);
				log_trace(trace_log_Config_Files,"Archivo de Configuracion actualizado y levantado.\n");
			}
			config_destroy(ptrConfigUpdate);
		}
	}

	liberaVariables(trace_log_Config_Files, ptrConfig, ptrConfigUpdate);

	free(ptrvaloresConfigFile);
};

void levantaConfigFileEnVariables(KernelConfigFile *ptrvaloresConfigFile,t_config *ptrConfig){
	ptrvaloresConfigFile->puerto_programa = leer_string(ptrConfig, "PUERTO_UMC");
	ptrvaloresConfigFile->puerto_cpu = leer_string(ptrConfig, "SERVER_PORT");
	ptrvaloresConfigFile->quantum = leerUnsigned(ptrConfig, "QUANTUM");
	ptrvaloresConfigFile->quantum_sleep = leerUnsigned(ptrConfig, "QUANTUM_SLEEP");
	ptrvaloresConfigFile->idDispositivo = leerArray(ptrConfig, "IO_ID");
	ptrvaloresConfigFile->sleepIO = leerArray(ptrConfig, "IO_SLEEP");
	ptrvaloresConfigFile->idSemaforo = leerArray(ptrConfig, "SEM_ID");
	ptrvaloresConfigFile->inicioSemaforo = leerArray(ptrConfig, "SEM_INIT");
	ptrvaloresConfigFile->globalVar = leerArray(ptrConfig, "SHARED_VARS");
	ptrvaloresConfigFile->stack_size = leerUnsigned(ptrConfig,"STACK_SIZE");
	ptrvaloresConfigFile->umc_ip = leer_string(ptrConfig,"UMC_IP");
	ptrvaloresConfigFile->server_ip = leer_string(ptrConfig,"SERVER_IP");
	ptrvaloresConfigFile->backlog = leerUnsigned(ptrConfig,"BACKLOG");
}

void carga_variables_globales(KernelConfigFile *una_config){
	umc_ip = una_config->umc_ip;
	puerto_umc = una_config->puerto_programa;
	server_ip = una_config->server_ip;
	server_port = una_config->puerto_cpu;
}



void cargar_kernel(KernelConfigFile *config, t_kernel *kernel){

	kernel->quantum = config->quantum;
	kernel->quantum_sleep = config->quantum_sleep;
};


void detectaCambiosEnConfigFile() {
		char buffer[BUF_LEN];
		// Al inicializar inotify este nos devuelve un descriptor de archivo
		int file_descriptor = inotify_init();
		if (file_descriptor < 0) {
			perror("inotify_init");
		}
		// Creamos un monitor sobre un path indicando que eventos queremos escuchar
		int watch_descriptor = inotify_add_watch(file_descriptor,
				"./kernel_config.txt", IN_MODIFY);

		int length = read(file_descriptor, buffer, BUF_LEN);
		if (length < 0) {
			error_show("Error al leer el descriptor de archivo");
		}
		int offset = 0;

		while (offset < length) {

			struct inotify_event *event = (struct inotify_event *) &buffer[offset];

			if (event->len) {

				if (event->mask & IN_MODIFY) {
					if (event->mask) {
						huboUnCambio = 1;
					}
					}
				}
			offset += sizeof(struct inotify_event) + event->len;
			}

		inotify_rm_watch(file_descriptor, watch_descriptor);
		close(file_descriptor);
}

void liberaVariables(t_log* trace_log, t_config* ptrConfig, t_config* ptrConfigUpdate) {
	//Libera Logs y el Config File
	log_trace(trace_log, "Se libera el Archivo de Configuracion.\n");
	config_destroy(ptrConfig);
	//config_destroy(ptrConfigUpdate); // TODO MKN EZE
	log_trace(trace_log, "Se libera el Trace Log.\n");
	log_destroy(trace_log);
}

char* leer_string(t_config *config, char* key) {
	char * datoString = malloc(sizeof(config_get_string_value(config, key)));
	if (config_has_property(config, key)) {
		datoString = config_get_string_value(config, key);
	} else {
		error_show("No se leyó el %s de la config \n", key);
	}
	return datoString;
}

unsigned leerUnsigned(t_config *config, char* key) {
	unsigned datoInt = 0;
	if (config_has_property(config, key)) {
		datoInt = config_get_int_value(config, key);
	} else {
		error_show("No se leyó el %s de la config \n", key);
	}

	return datoInt;
}

char** leerArray(t_config *config, char* key) {
	char **datoString = malloc(sizeof(config_get_array_value(config, key)));
	if (config_has_property(config, key)) {
		datoString = config_get_array_value(config, key);
	} else {
		error_show("No se leyó el %s de la config \n", key);
	}
	return datoString;
}
