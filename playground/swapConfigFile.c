/*
 * swapConfigFile.c
 *
 *  Created on: 8/5/2016
 *      Author: utnso
 */

/*
 * SwapConfigFile.c
 *
 *  Created on: 23/4/2016
 *      Author: utnso
 */

#include<commons/config.h>
#include<stdlib.h>
#include<stdio.h>
#include<commons/error.h>
#include<commons/log.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/inotify.h>

int huboUnCambio = 0;

#define EVENT_SIZE (sizeof (struct inotify_event) + 24)
#define BUF_LEN (1024 * EVENT_SIZE)

typedef struct swapConfigFile {
	unsigned puerto_escucha,
			cant_paginas,
			size_pagina,
			retardo_compactacion;
	char 	*nombre_swap;
} SwapConfigFile;

char* leerString(t_config *config, char* key);
unsigned leerUnsigned(t_config *config, char* key);
void levantaConfigFileEnVariables(SwapConfigFile *ptrvaloresConfigFile,t_config *ptrConfig);
void liberaVariables(t_log* traceLogger, t_config* ptrConfig, t_log* errorLogger);
void detectaCambiosEnConfigFile();

int swapConfigFile()
{

	//Crea Logs
	t_log *errorLogger, *traceLogger;
	traceLogger = log_create("LogTraceUMC.txt","UMCConfigFile",true,LOG_LEVEL_TRACE);
	errorLogger = log_create("LogErroresUMC.txt","UMCConfigFile",true,LOG_LEVEL_ERROR);

	//Declaracion de Variables
	t_config *ptrConfig, *ptrConfigUpdate;
	SwapConfigFile *ptrvaloresConfigFile;
	ptrvaloresConfigFile = NULL;
	ptrvaloresConfigFile = malloc(sizeof(SwapConfigFile));
	ptrConfigUpdate = NULL;

	//Se asigna a ptrConfig el archivo de configuracion. Si no lo encuentra, finaliza
	//y lo advierte en el log
	ptrConfig = config_create("/home/utnso/workspace/SwapConfigFile/Debug/config.txt");
	if (ptrConfig == NULL){
		log_error(errorLogger,"Archivo de configuración no disponible. No puede ejecutar el UMC.\n");
		return EXIT_FAILURE;
	} else {
		log_trace(traceLogger,"Iniciando Proceso UMC.\n");
	}

	//El procedimiento carga los valores del Config File en las variables creadas
	levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfig);

	log_trace(traceLogger,"Archivo de Configuracion levantado exitosamente.\n");

	printf("%d\n\n", ptrvaloresConfigFile->puerto_escucha);

	while (1)
	{
		huboUnCambio=0;
		detectaCambiosEnConfigFile();
		if (huboUnCambio)
		{
			ptrConfigUpdate = config_create("/home/utnso/workspace/SwapConfigFile/Debug/config.txt");
			if(ptrConfigUpdate->properties->elements_amount==0) {
				log_error(errorLogger,"No se puede levantar el Archivo de Configuracion.\n");
			} else {
				levantaConfigFileEnVariables(ptrvaloresConfigFile, ptrConfigUpdate);
				printf("%d\n\n", ptrvaloresConfigFile->puerto_escucha);
				log_trace(traceLogger,"Archivo de Configuracion actualizado y levantado.\n");
			}
			config_destroy(ptrConfigUpdate);

		}
	}

	printf("%s\n\n", ptrvaloresConfigFile->nombre_swap);

	liberaVariables(traceLogger, ptrConfig, errorLogger);
	free(ptrvaloresConfigFile);

	return EXIT_SUCCESS;

}; //Fin de Main

char* leerString(t_config *config, char* key) {
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

void levantaConfigFileEnVariables(SwapConfigFile *ptrvaloresConfigFile,t_config *ptrConfig){
	ptrvaloresConfigFile->puerto_escucha = leerUnsigned(ptrConfig, "PUERTO_ESCUCHA");
	ptrvaloresConfigFile->nombre_swap = leerString(ptrConfig, "NOMBRE_SWAP");
	ptrvaloresConfigFile->cant_paginas = leerUnsigned(ptrConfig, "CANTIDAD_PAGINAS");
	ptrvaloresConfigFile->size_pagina = leerUnsigned(ptrConfig, "SIZE_PAGINA");
	ptrvaloresConfigFile->retardo_compactacion = leerUnsigned(ptrConfig, "RETARDO_COMPACTACION");
}
void liberaVariables(t_log* traceLogger, t_config* ptrConfig, t_log* errorLogger) {
	//Libera Logs y el Config File
	log_trace(traceLogger, "Se libera el Archivo de Configuracion.\n");
	config_destroy(ptrConfig);
	log_trace(traceLogger, "Se libera el Log de Errores.\n");
	log_destroy(errorLogger);
	log_trace(traceLogger, "Se libera el Trace Log.\n");
	log_destroy(traceLogger);
}
void detectaCambiosEnConfigFile() {
		char buffer[BUF_LEN];
		// Al inicializar inotify este nos devuelve un descriptor de archivo
		int file_descriptor = inotify_init();
		if (file_descriptor < 0) {
			perror("inotify_init");
		}
		// Creamos un monitor sobre un path indicando que eventos queremos escuchar
		int watch_descriptor = inotify_add_watch(file_descriptor,
				"/home/utnso/workspace/SwapConfigFile/Debug", IN_MODIFY);

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
			huboUnCambio = 0;
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
						printf("The file %s was modified.\n\n", event->name);
						huboUnCambio = 1;
					}
					}
				}
			offset += sizeof(struct inotify_event) + event->len;
			}

		inotify_rm_watch(file_descriptor, watch_descriptor);
		close(file_descriptor);
}

