/*
 * levanta_config_files.c
 *
 *  Created on: 20/6/2016
 *      Author: utnso
 */

#include "main.h"
#include "levanta_config_files.h"
#include <commons/config.h>




int *cargar_configuracion(){
	t_log *trace_log_Config_Files;
	trace_log_Config_Files = log_create("./Log_de_Config_Files.txt","main.c",false,LOG_LEVEL_TRACE);

	t_config *ptrConfig, *ptrConfigUpdate;

	UMCConfigFile *ptrvaloresConfigFile = malloc(sizeof(UMCConfigFile));

	ptrConfig = config_create("./umc.cfg");
	if (ptrConfig == NULL){
		log_trace(trace_log_Config_Files,"Archivo de configuración no disponible. No puede ejecutar el UMC.\n");
		return -1;
	}

	log_trace(trace_log_Config_Files,"Cargando parametros del Archivo de Configuracion\n");

	levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfig);
	cargar_variables_productivas(ptrvaloresConfigFile);
	log_trace(trace_log_Config_Files,"Archivo de Configuracion levantado exitosamente.\n");
	set_configuracion_cargada();
	sem_post(&sem_config_file_umc);
	while(1)
	{
		extern huboUnCambio;
		huboUnCambio = 0;
		detectaCambiosEnConfigFile();
		if(huboUnCambio)
		{
			ptrConfigUpdate = config_create("./umc.cfg");
			if(ptrConfigUpdate->properties->elements_amount==0) {
				log_trace(trace_log_Config_Files,"No se puede levantar el Archivo de Configuracion\n");
			} else {
				levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfigUpdate);
				cargar_variables_productivas(ptrvaloresConfigFile);
				log_trace(trace_log_Config_Files,"Archivo de Configuracion actualizado y levantado.\n");
			}
			config_destroy(ptrConfigUpdate);
		}

	}

	liberaVariables(trace_log_Config_Files, ptrConfig, ptrConfigUpdate);

	free(ptrvaloresConfigFile);
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
				"./umc.cfg", IN_MODIFY);

		// El file descriptor creado por inotify, es el que recibe la información sobre los eventos ocurridos
		// para leer esta información el descriptor se lee como si fuera un archivo comun y corriente pero
		// la diferencia esta en que lo que leemos no es el contenido de un archivo sino la información
		// referente a los eventos ocurridos
		int length = read(file_descriptor, buffer, BUF_LEN);
		if (length < 0) {
			error_show("Error al leer el descriptor de archivo");
		}
		int offset = 0;

		// Luego del read buffer es un array de n posiciones donde cada posición contiene
		// un eventos ( inotify_event ) junto con el nombre de este.
		while (offset < length) {

			// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
			// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
			// a sizeof( struct inotify_event ) + 24.
			struct inotify_event *event = (struct inotify_event *) &buffer[offset];
			// El campo "len" nos indica la longitud del tamaño del nombre
			if (event->len) {
				// Dentro de "mask" tenemos el evento que ocurrio y sobre donde ocurrio
				// sea un archivo o un directorio
				if (event->mask & IN_MODIFY) {
					if (event->mask) {
						extern huboUnCambio;
						huboUnCambio = 1;
					}
					}
				}
			offset += sizeof(struct inotify_event) + event->len;
			}

		inotify_rm_watch(file_descriptor, watch_descriptor);
		close(file_descriptor);
}

void levantaConfigFileEnVariables(UMCConfigFile *ptrvaloresConfigFile,t_config *ptrConfig){
	ptrvaloresConfigFile->puerto = leer_string(ptrConfig, "PUERTO");
	ptrvaloresConfigFile->puerto_swap = leer_string(ptrConfig, "PUERTO_SWAP");
	ptrvaloresConfigFile->marcos = leerUnsigned(ptrConfig, "MARCOS");
	ptrvaloresConfigFile->marcos_size = leerUnsigned(ptrConfig, "MARCOS_SIZE");
	ptrvaloresConfigFile->marco_x_proc = leerUnsigned(ptrConfig, "MARCO_X_PROC");
	ptrvaloresConfigFile->entradas_tlb = leerUnsigned(ptrConfig, "ENTRADAS_TLB");
	ptrvaloresConfigFile->retardo = leerUnsigned(ptrConfig, "RETARDO");
	ptrvaloresConfigFile->ip_swap = leer_string(ptrConfig, "IP_SWAP");
	ptrvaloresConfigFile->algoritmo_reemplazo = leer_string(ptrConfig, "ALGORITMO");
	ptrvaloresConfigFile->ip_server = leer_string(ptrConfig, "IP_SERVER");
};

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
