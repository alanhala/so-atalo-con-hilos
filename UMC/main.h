/*
 * main.h
 *
 *  Created on: 17/5/2016
 *      Author: utnso
 */

#include <commons/log.h>
#include <commons/config.h>
#include "levanta_config_files.h"

int  LISTENPORT;
int SWAPPORT;
char * SWAPIP;

void set_configuracion_cargada();
int get_configuracion_cargada();
