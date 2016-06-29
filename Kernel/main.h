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
		*puerto_umc,
		*server_ip,
		*server_port;

sem_t sem_config_file_kernel;



#endif /* MAIN_H_ */
