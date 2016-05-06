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

#define UMCPORT "21000"
#define UMCIP "localhost"

void connect_to_UMC();
void connect_to_Kernel();
void *connect_to_UMC_thread();
void *connect_to_kernel_thread(void);

int

main(int argc, char **argv) {
	connect_to_UMC();
	connect_to_Kernel();

	while (1) { // para que no se cierren los threads
		sleep(10);
	}

	return 0;
}

void connect_to_UMC() {
	pthread_t thread;
	int thread_result = pthread_create(&thread, NULL, &connect_to_UMC_thread,
			NULL);

	if (thread_result) {
		// TODO LOGUEAR ERROR
		// TODO Analizar el tratamiento que desea darse
		printf("Error - pthread_create() return code: %d\n", thread_result);
		exit(1);
	}

}

void *connect_to_UMC_thread() {
	int umc_socket_descriptor = create_client_socket_descriptor(UMCIP,
			UMCPORT);

	while (1) { //TODO REVISAR por que hace print de lo que envia y de lo que recibe. puede que ser que sea por el socket?
			//char header[1];
			char recvBuffer[15];
			send(umc_socket_descriptor, "holaa umc, soy cpu\n", 19, 0);
			//recv(swap_socket_descriptor, header, 2, 0);
			//char recvBuffer[header];
			recv(umc_socket_descriptor, recvBuffer, 15, 0);

			printf(recvBuffer);
			fflush(stdout);
			sleep(1);
		}
}

void connect_to_Kernel() {
	pthread_t thread;
	int thread_result = pthread_create(&thread, NULL, &connect_to_kernel_thread,
			NULL);

	if (thread_result) {
		// TODO LOGUEAR ERROR
		// TODO Analizar el tratamiento que desea darse
		printf("Error - pthread_create() return code: %d\n", thread_result);
		exit(1);
	}
}

void *connect_to_kernel_thread(void) {
	create_client_socket_descriptor("localhost", KERNELPORT);
}
