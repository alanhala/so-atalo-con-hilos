/*
 * console_main.c
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
#include "protocoloConsola.h"
#include <commons/log.h>
#include <commons/config.h>

char	*kernel_ip,
		*kernel_puerto;

int kernel_socket_descriptor;

int levanta_config_consola(void);

//Agrega Newton -- Inicio
//void cargaArray(char array[],  FILE* codeF);
//void imprimeArray(const int cantCar,char array[]);
//Agrega Newton -- Fin

void listen_sigint_signal();
void carga_programas_ansisop(char *programas[15]);

int main(int argc, char **argv) {

	levanta_config_consola();
	t_log *trace_log = log_create("./Log_de_Consola.txt", "console_main.c",
		true, LOG_LEVEL_TRACE);

	log_trace(trace_log,"\n\n\n\n\n");

	char *array_de_programas[15];
	char *codigo;

	FILE *fdarchivo;


	if ((fdarchivo = fopen(argv[1], "r")) == 0) {
		//todo log el archivo esta vacio en este caso no se ejecuta consola

	} else {
		fseek(fdarchivo, 0L, SEEK_END);
		int tamanio = ftell(fdarchivo);
		char * buff = malloc(255);
		rewind(fdarchivo);
		fgets(buff, 255, fdarchivo);
		int inicio =  ftell(fdarchivo);
		tamanio = tamanio - inicio;
		codigo = malloc(tamanio);
		fread(codigo, tamanio, inicio, fdarchivo);
		log_trace(trace_log,"%s\n", codigo);
		free(buff);
		fclose(fdarchivo);
	}


//	carga_programas_ansisop(array_de_programas);
//
//	codigo = array_de_programas[4];



	kernel_socket_descriptor = create_client_socket_descriptor(kernel_ip,
			kernel_puerto);
	int a = 2;
	send(kernel_socket_descriptor, &a, sizeof(int), 0);

	t_iniciar_programa_en_kernel *iniciar_programa = malloc(sizeof(t_iniciar_programa_en_kernel));
	memset(iniciar_programa,0,sizeof(t_iniciar_programa_en_kernel));

	iniciar_programa->codigo_de_programa = malloc(strlen(codigo)+1);
	memcpy(iniciar_programa->codigo_de_programa, codigo, strlen(codigo)+1);

	t_stream *buffer = malloc(sizeof(t_stream));

	buffer = serializar_mensaje(91,iniciar_programa);

	log_trace(trace_log,"Enviando el codigo ansisop al Nucleo\n");

	send(kernel_socket_descriptor,buffer->datos,buffer->size,0);

	t_header *un_header = malloc(sizeof(t_header));

	char buffer_header[5];

	int	bytes_recibidos_header,
		bytes_recibidos;

	log_trace(trace_log,"Recibiendo la respuesta del Nucleo\n");
	bytes_recibidos_header = recv(kernel_socket_descriptor,buffer_header,5,MSG_PEEK);

	un_header = deserializar_header(buffer_header);

	char buffer_recibidos[un_header->length];

	bytes_recibidos = recv(kernel_socket_descriptor,buffer_recibidos,un_header->length,0);

	t_respuesta_iniciar_programa_en_kernel *respuesta = malloc(sizeof(t_respuesta_iniciar_programa_en_kernel));

	respuesta = deserealizar_mensaje(92,buffer_recibidos);
	if(respuesta->respuesta_correcta == 22){
		log_trace(trace_log,"Inicio correcto de programa ansisop\n");
		pthread_t signal_listener;
		pthread_create(&signal_listener, NULL, &listen_sigint_signal, NULL);
	}else{
		log_trace(trace_log,"No se pudo iniciar el programa ansisop\n");
	}

	while (1) {
		t_header *un_header = malloc(sizeof(t_header));
		char buffer_header[5];

		int	bytes_recibidos_header,
			bytes_recibidos;

		bytes_recibidos_header = recv(kernel_socket_descriptor,buffer_header,5,MSG_PEEK);

		un_header = deserializar_header(buffer_header);

		char buffer_recibidos[(un_header->length)];

		if(un_header->tipo == 132){

			log_trace(trace_log,"Recibiendo el codigo a imprimir pedido por CPU\n");
			int bytes_recibidos = recv(kernel_socket_descriptor,buffer_recibidos,un_header->length,0);

			t_imprimir_texto_en_consola *texto_a_imprimir = malloc(sizeof(t_imprimir_texto_en_consola));

			texto_a_imprimir = (t_imprimir_texto_en_consola *)deserealizar_mensaje(132,buffer_recibidos);

			printf("%s\n",texto_a_imprimir->texto_a_imprimir);
			fflush(stdout);

		}
		if(un_header->tipo == 133){

			log_trace(trace_log,"Recibiendo la finalizacion de la Consola\n");
			int bytes_recibidos = recv(kernel_socket_descriptor,buffer_recibidos,un_header->length,0);

			t_finalizar_programa_en_consola *finalizar = malloc(sizeof(t_finalizar_programa_en_consola));

			finalizar = (t_finalizar_programa_en_consola *)deserealizar_mensaje(133,buffer_recibidos);

			if (finalizar->motivo == 1){

				log_trace(trace_log,"El programa finalizo correctamente\n");
				fflush(stdout);
			} else if (finalizar->motivo == 2){

				log_trace(trace_log,"El programa finalizo por Stack Overflow\n");
				fflush(stdout);
			} else if (finalizar->motivo == 7) {

				log_trace(trace_log, "Insuficiente espacio como para cargar el programa\n");
				fflush(stdout);
			} else if (finalizar->motivo == 58) {

				log_trace(trace_log, "No hay frames libres, se finaliza el proceso\n");
				fflush(stdout);
			} else if (finalizar->motivo == 34) {

				log_trace(trace_log, "El proceso no puede continuar su ejecucion ya que se desconecto la CPU en medio de la rafaga\n");
				fflush(stdout);
			}
			else {

				log_trace(trace_log,"El programa no finalizo correctamente\n");
				fflush(stdout);
			}
			break;
		}
	}

	log_destroy(trace_log);

	return 0;
}

void sigint_handler() {
	printf("SIGINT");
	fflush(stdout);
	int a = 1;
	send(kernel_socket_descriptor, &a, sizeof(int), 0);
	exit(1);
}

void carga_programas_ansisop(char *programas[15]){

	/*
	 * 0 - Fibo
	 * 1 - Consumidor
	 * 2 - Productor
	 * 3 - Vector
	 * 4 - Completo
	 */
	programas[0]="begin\n	variables x\n	x <- fibo 50\n	# Esperable: SegFault en el 10mo (40)\n	\n	# x <- fibo 8\n	#Esperable: 21\n	\n	textPrint Solucion:\n	print x\n	end\n	\n	function fibo\n	print $0\n	jz $0 return0\n	jz $0-1 return1\n	variables a, b\n	a <- fibo $0-1\n	b <- fibo $0-2\n	return a+b\n	\n	:return0\n	return 0\n	\n	:return1\n	return 1\n";
	programas[1]="begin\n	:etiqueta\n	\n	wait c\n	print !colas\n	signal b\n	\n	#Ciclar indefinidamente\n	goto etiqueta\n	\n	end\n";
	programas[2]="begin\n	:etiqueta\n	\n	wait b\n	!colas = !colas +1\n	signal c\n	\n	#Ciclar indefinidamente\n	goto etiqueta\n	\n	end\n";
	programas[3]="begin\n	#un vector de 5 posiciones\n	variables a, b, c, d, e, i\n	#No inicializo las primeras 5 variables (vector) para tener elementos aleatorios\n	#i va a ser mi iterador, me interesa que empieze en 0\n	i=0\n	\n	#Bucle del for\n	:for\n	#imprime el valor iavo del vector\n	print *&a+i\n	#pongo en 0 el valor de la posicion para verificar escritura\n	*&a+i = 0\n	#avanzo en el vector (de a 4 posiciones, 1 int)\n	i=i+4\n	#Si i no es 20 (5 posiciones del vector * 4 temanio de las variables)\n	jnz 20-i for\n	\n	#Final del bucle\n	textPrint Fin\n	end\n";
	programas[4]="begin\n	variables f, A, g\n	A = 0\n	!compartida = 1+A\n	print !compartida\n	jnz !compartida Siguiente\n	:Proximo\n	\n	f = 8\n	g <- doble !compartida\n	io LPT1 20\n	\n	textPrint Hola Mundo!\n	\n	g = 1 + g\n	print g\n	\n	textPrint Bye\n	\n	end\n	\n	\n	#Devolver el doble del\n	#primer parametro\n	function doble\n	variables f\n	f = $0 + $0\n	return f\n	end\n	\n	:Siguiente\n	print A+1\n	goto Proximo\n";
}

void listen_sigint_signal() {
	if (signal(SIGINT, sigint_handler) == SIG_ERR)
		printf("\ncan't catch SIGINT\n");
}

int levanta_config_consola(void){

	t_config *config_consola = config_create("./config_console.txt");

	if(config_consola==NULL){
		//TODO loggear error
		return 1;
	}

	kernel_ip = config_get_string_value(config_consola,"IP_KERNEL");
	kernel_puerto = config_get_string_value(config_consola,"PUERTO_KERNEL");

	return 0;
}
