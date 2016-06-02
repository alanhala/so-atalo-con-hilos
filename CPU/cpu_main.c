/*
 * cpu_main.c
 *
 *  Created on: 28/4/2016
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define KERNELPORT "8002"
#define KERNELIP "localhost"

#define UMCPORT "5000"
#define UMCIP "localhost"


void connect_to_UMC();
void connect_to_Kernel();


int main(int argc, char **argv) {

	/*if (argc == 3) {
	    if (strcmp(argv[2], "-conexiones") == 0){
    		connect_to_UMC();
    		//connect_to_Kernel();
    	    }
	}*/

	if (strcmp(argv[1], "-test") == 0){
		 correrTest();
		 //correrTestSerializacion();
		 return 0;
	}

	connect_to_UMC();
	    	connect_to_Kernel();

	while (1) { // para que no se cierren los threads
		sleep(10);
	}

	return 0;
}

void connect_to_UMC() {

	int umc_socket_descriptor = create_client_socket_descriptor(UMCIP, UMCPORT);

	set_umc_socket_descriptor(umc_socket_descriptor);


}



void connect_to_Kernel() {

	int kernel_socket_descriptor =create_client_socket_descriptor("localhost", KERNELPORT);

	set_kernel_socket_descriptor(kernel_socket_descriptor);
}

