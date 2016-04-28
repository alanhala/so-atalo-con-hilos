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

#define KERNELPORT "30000"
#define KERNELIP "localhost"

#define UMCPORT "21000"
#define UMCIP "localhost"


void connect_to_UMC();
void connect_to_Kernel();

int main(int argc, char **argv) {
	void connect_to_UMC();
	void connect_to_Kernel();
	return 0;
}

void connect_to_UMC(){

}


void connect_to_Kernel(){

}
