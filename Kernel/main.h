/*
 * main.h
 *
 *  Created on: 28/6/2016
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <semaphore.h>

uint32_t	backlog;

char	*umc_ip,
		*puerto_programa,
		*server_ip,
		*puerto_cpu;

sem_t sem_config_file_kernel;



#endif /* MAIN_H_ */
