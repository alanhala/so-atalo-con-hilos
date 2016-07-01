/*
 * main.h
 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */

#include <commons/log.h>
#include <commons/config.h>
#include "levanta_config_files.h"
#include <semaphore.h>

int LISTENPORT;
int SWAPPORT;
char * SWAPIP;



sem_t sem_config_file_umc;

void set_configuracion_cargada();
int get_configuracion_cargada();
