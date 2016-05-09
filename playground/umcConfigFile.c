/*
 * umcConfigFile.c
 *
 *  Created on: 8/5/2016
 *      Author: utnso
 */

/*
 * UMCConfigFile.c
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

int huboUnCambio;

#define EVENT_SIZE (sizeof (struct inotify_event) + 24)
#define BUF_LEN (1024 * EVENT_SIZE)

typedef struct umcConfigFile {
	unsigned puerto,
			puerto_swap,
			marcos,
			marcos_size,
			marco_x_proc,
			entradas_tlb,
			retardo;
	char 	*ip_swap;
} UMCConfigFile;

char* leerString(t_config *config, char* key);
unsigned leerUnsigned(t_config *config, char* key);

void levantaConfigFileEnVariables(UMCConfigFile *ptrvaloresConfigFile,t_config *ptrConfig);
void liberaVariables(t_log* traceLogger, t_config* ptrConfig, t_log* errorLogger, t_config* ptrConfigUpdate);
void detectaCambiosEnConfigFile();

int main()
{

	//Crea Logs
	t_log *errorLogger, *traceLogger;
	traceLogger = log_create("LogTraceUMC.txt","UMCConfigFile",true,LOG_LEVEL_TRACE);
	errorLogger = log_create("LogErroresUMC.txt","UMCConfigFile",true,LOG_LEVEL_ERROR);

	//Declaracion de Variables
	t_config *ptrConfig, *ptrConfigUpdate;
	UMCConfigFile *ptrvaloresConfigFile;
	ptrvaloresConfigFile = malloc(sizeof(UMCConfigFile));

	//Se asigna a ptrConfig el archivo de configuracion. Si no lo encuentra, finaliza
	//y lo advierte en el log
	ptrConfig = config_create("/home/utnso/workspace/UMCConfigFile/Debug/config.txt");
	if (ptrConfig == NULL){
		log_error(errorLogger,"Archivo de configuración no disponible. No puede ejecutar el UMC.\n");
		return EXIT_FAILURE;
	}

	log_trace(traceLogger,"Iniciando Proceso UMC.\n");

	//El procedimiento carga los valores del Config File en las variables creadas
	levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfig);

	log_trace(traceLogger,"Archivo de Configuracion levantado exitosamente.\n");

	printf("%s\n\n", ptrvaloresConfigFile->ip_swap);

	while(1)
	{
		huboUnCambio=0;
		detectaCambiosEnConfigFile();
		if(huboUnCambio)
		{
			ptrConfigUpdate = config_create("/home/utnso/workspace/UMCConfigFile/Debug/config.txt");
			if(ptrConfigUpdate->properties->elements_amount==0) {
				log_error(errorLogger,"No se puede levantar el Archivo de Configuracion.\n");
			} else {
				levantaConfigFileEnVariables(ptrvaloresConfigFile,ptrConfigUpdate);
				printf("%s\n\n", ptrvaloresConfigFile->ip_swap);
				log_trace(traceLogger,"Archivo de Configuracion actualizado y levantado.\n");
			}
			config_destroy(ptrConfigUpdate);
		}
	}
	printf("%s\n\n", ptrvaloresConfigFile->ip_swap);

	liberaVariables(traceLogger, ptrConfig, errorLogger, ptrConfigUpdate);
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
void levantaConfigFileEnVariables(UMCConfigFile *ptrvaloresConfigFile,t_config *ptrConfig){
	ptrvaloresConfigFile->puerto = leerUnsigned(ptrConfig, "PUERTO");
	ptrvaloresConfigFile->puerto_swap = leerUnsigned(ptrConfig, "PUERTO_SWAP");
	ptrvaloresConfigFile->marcos = leerUnsigned(ptrConfig, "MARCOS");
	ptrvaloresConfigFile->marcos_size = leerUnsigned(ptrConfig, "MARCOS_SIZE");
	ptrvaloresConfigFile->marco_x_proc = leerUnsigned(ptrConfig, "MARCO_X_PROC");
	ptrvaloresConfigFile->entradas_tlb = leerUnsigned(ptrConfig, "ENTRADAS_TLB");
	ptrvaloresConfigFile->retardo = leerUnsigned(ptrConfig, "RETARDO");
	ptrvaloresConfigFile->ip_swap = leerString(ptrConfig, "IP_SWAP");
}
void liberaVariables(t_log* traceLogger, t_config* ptrConfig, t_log* errorLogger, t_config* ptrConfigUpdate) {
	//Libera Logs y el Config File
	log_trace(traceLogger, "Se libera el Archivo de Configuracion.\n");
	config_destroy(ptrConfig);
	config_destroy(ptrConfigUpdate);
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
				"/home/utnso/workspace/UMCConfigFile/Debug", IN_MODIFY);

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

