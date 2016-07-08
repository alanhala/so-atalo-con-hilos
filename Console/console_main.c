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
int sigusr_received;

void sig_handler(){
	printf("SIGUSR1 capturada");
	fflush(stdout);
	//close(kernel_socket_descriptor);
	kill(getpid(),9);
};
void *captador_de_senal_thread(){

	//printf("Process ID: %d\n",getpid());
	fflush(stdout);

	if(signal(SIGUSR1,sig_handler) == SIG_ERR)
		error_show("No se atrapo la senal\n");

}

//void listen_sigint_signal();

int main(int argc, char **argv) {

	levanta_config_consola();
	pthread_t captador_de_senal;
	pthread_create(&captador_de_senal,NULL,&captador_de_senal_thread,NULL);

	char* codigo;
//	FILE *fdarchivo;
//
//	if ((fdarchivo = fopen(argv[1], "rb")) == 0) {
//		//todo log el archivo esta vacio en este caso no se ejecuta consola
////return 0;PESADO ANSISOP
//
//		//Pesado Ansisop
//		//codigo = "begin\ngoto Etiqueta19\nend\n:Etiqueta1\n	:Etiqueta2\n	:Etiqueta3\n	:Etiqueta4\n	:Etiqueta5\n	:Etiqueta6\n	:Etiqueta7\n	:Etiqueta8\n	:Etiqueta9\n	:Etiqueta10\n	:Etiqueta11\n	:Etiqueta12\n	:Etiqueta13\n	:Etiqueta14\n	:Etiqueta15\n	:Etiqueta16\n	:Etiqueta17\n	:Etiqueta18\n	:Etiqueta19\n	:Etiqueta20\n	:Etiqueta21\n	:Etiqueta22\n	:Etiqueta23\n	:Etiqueta24\n	:Etiqueta25\n	:Etiqueta26\n	:Etiqueta27\n	textPrint Alfin entre!\n	end\n";
//
//		//Vector Ansisop
//		//codigo = "begin\n	#un vector de 5 posiciones\n	variables a, b, c, d, e, i\n	#No inicializo las primeras 5 variables (vector) para tener elementos aleatorios\n	#i va a ser mi iterador, me interesa que empieze en 0\n	i=0\n	\n	#Bucle del for\n	:for\n	#imprime el valor iavo del vector\n	print *&a+i\n	#pongo en 0 el valor de la posicion para verificar escritura\n	*&a+i = 0\n	#avanzo en el vector (de a 4 posiciones, 1 int)\n	i=i+4\n	#Si i no es 20 (5 posiciones del vector * 4 temanio de las variables)\n	jnz 20-i for\n	\n	#Final del bucle\n	textPrint Fin\n	end\n";
//
//		//For Ansisop
//		//codigo = "begin\n	variables f, i, t\n	\n	#`f`: Hasta donde contar\n	f=20\n	#`i`: Iterador\n	i=0\n	\n	:inicio\n	\n	#Iterar\n	i=i+1\n	\n	#Imprimir el contador\n	print i\n	\n	#`t`: Comparador entre `i` y `f`\n	t=f-i\n	#De no ser iguales, salta a inicio\n	jnz t inicio\n	\n	end\n";
//
//		//Fibo Ansisop
//		//codigo = "begin\n	variables x\n	x <- fibo 3\n	# Esperable: SegFault en el 10mo (40)\n	\n	# x <- fibo 8\n	#Esperable: 21\n	\n	textPrint Solucion:\n	print x\n	end\n	\n	function fibo\n	print $0\n	jz $0 return0\n	jz $0-1 return1\n	variables a, b\n	a <- fibo $0-1\n	b <- fibo $0-2\n	return a+b\n	\n	:return0\n	return 0\n	\n	:return1\n	return 1\n";
//		//codigo = "begin\n variables x\n x <- fibo 8\n # Esperable: SegFault en el 10mo (40)\n \n # x <- fibo 8\n #Esperable: 21\n \n textPrint Solucion:\n print x\n end\n \n function fibo\n print $0\n jz $0 return0\n jz $0-1 return1\n variables a, b\n a <- fibo $0-1\n b <- fibo $0-2\n return a+b\n \n :return0\n return 0\n \n :return1\n return 1\n";
//		codigo =   "begin\nvariables x\nx <- fibo 8\n # Esperable: SegFault en el 10mo (40)\n \n # x <- fibo 8\n #Esperable: 21\n\ntextPrint Solucion:\nprint x\nend\n\nfunction fibo\nprint $0\njz $0 return0\njz $0-1 return1\nvariables a, b\na <- fibo $0-1\nb <- fibo $0-2\nreturn a+b\n\n:return0\nreturn 0\n:return1\nreturn 1\n";
////return 0;
////		codigo = "begin\nvariables c, d, e\nc=2147483647\nd=224947129\nf\ne <- g\nend\nfunction f\nvariables a\na=1\nend\nfunction g\nvariables a\na=2\nreturn a\nend";
//
//
//	} else {
//		fseek(fdarchivo, 0, SEEK_END);
//		int tamanio = ftell(fdarchivo);
//		rewind(fdarchivo);
//		codigo = malloc(tamanio);
//		fread(codigo, tamanio, 1, fdarchivo);
//		printf(" %s\n", codigo);
//	}

	t_log *trace_log = log_create("./Log_de_Consola.txt", "console_main.c",
	true, LOG_LEVEL_TRACE);
	kernel_socket_descriptor = create_client_socket_descriptor(kernel_ip,
			kernel_puerto);
	int a = 2;
	send(kernel_socket_descriptor, &a, sizeof(int), 0);

	t_iniciar_programa_en_kernel *iniciar_programa = malloc(sizeof(t_iniciar_programa_en_kernel));
	memset(iniciar_programa,0,sizeof(t_iniciar_programa_en_kernel));

	//char * codigo = "begin\nvariables c, d\nc=1234\nd=4321\nend\0";
//	char * codigo = "begin\nvariables c, d, e\nc=2147483647\nd=224947129\nf\ne <- g\nend\nfunction f\nvariables a\na=1234\nend\nfunction g\nvariables a\na=4321\nreturn a\nend";
//	char * codigo = "begin\nvariables c, d, e\nc=2147483647\nd=224947129\nf\ne <- g\niterar\nend\nfunction f\nvariables a\na=2\nprint a\nend\nfunction g\nvariables a\na=2\nreturn a\nend\nfunction iterar\nvariables f, i, t\nf=20\ni=0\n:inicio\ni=i+1\nprint i\nt=f-i\nprint t\njnz t inicio\nend";
//	char* codigo = "begin\nvariables a, b\na = 3\nb = 5\na = b + 12\nend";
 //codigo = "begin\nvariables c, d, e\nc=2147483647\nd=20\nf\ne <- g\niterar\nrecursiva d\ntextPrint Finaliza programa\nend\nfunction f\nvariables a\na=1234\nend\nfunction g\nvariables a\na=2\nreturn a\nend\nfunction iterar\nvariables f, i, t\nf=20\ni=0\n:inicio\ni=i+1\nprint i\nt=f-i\nprint t\njnz t inicio\nend\nfunction recursiva\njz $0 salir\nvariables a\na = $0 - 1\ntextPrint recursiva\nprint a\nrecursiva a\n:salir\nend";


	//Pesado Ansisop
		//codigo = "begin\ngoto Etiqueta19\nend\n:Etiqueta1\n	:Etiqueta2\n	:Etiqueta3\n	:Etiqueta4\n	:Etiqueta5\n	:Etiqueta6\n	:Etiqueta7\n	:Etiqueta8\n	:Etiqueta9\n	:Etiqueta10\n	:Etiqueta11\n	:Etiqueta12\n	:Etiqueta13\n	:Etiqueta14\n	:Etiqueta15\n	:Etiqueta16\n	:Etiqueta17\n	:Etiqueta18\n	:Etiqueta19\n	:Etiqueta20\n	:Etiqueta21\n	:Etiqueta22\n	:Etiqueta23\n	:Etiqueta24\n	:Etiqueta25\n	:Etiqueta26\n	:Etiqueta27\n	textPrint Alfin entre!\n	end\n";
//
//		Vector Ansisop
	//codigo = "begin\n	#un vector de 5 posiciones\n	variables a, b, c, d, e, i\n	#No inicializo las primeras 5 variables (vector) para tener elementos aleatorios\n	#i va a ser mi iterador, me interesa que empieze en 0\n	i=0\n	\n	#Bucle del for\n	:for\n	#imprime el valor iavo del vector\n	print *&a+i\n	#pongo en 0 el valor de la posicion para verificar escritura\n	*&a+i = 0\n	#avanzo en el vector (de a 4 posiciones, 1 int)\n	i=i+4\n	#Si i no es 20 (5 posiciones del vector * 4 temanio de las variables)\n	jnz 20-i for\n	\n	#Final del bucle\n	textPrint Fin\n	end\n";


	//COMPLETO
	//codigo = "\n#Respuesta esperada: 1; 1; Hola Mundo!; 3; Bye\n\nbegin\nvariables f,  A,  g\n    A = 	0\n    !compartida = 1+A\n    print !compartida\n    jnz !compartida Siguiente \n:Proximo\n	\n    f = 8	  \n    g <- doble !compartida	\n    io LPT1 20\n\n    textPrint    Hola Mundo!\n    \n    g = 1 + g\n    print 		g    \n    \n    textPrint Bye\n    \nend\n\n\n#Devolver el doble del\n#primer parametro\nfunction doble\nvariables f\n    f = $0 + $0\n    return f\nend\n\n:Siguiente	\n	print A+1\ngoto Proximo\n\n";

	//PRODUCTOR
	//codigo="\nbegin	\n	:etiqueta\n	\n	wait b\n		!colas = !colas +1\n	signal c\n	\n	#Ciclar indefinidamente\n	goto etiqueta\n\nend\n\n";

	//CONSUMIDOR
	//codigo = "\nbegin	\n	:etiqueta\n	\n	wait c\n		print !colas\n	signal b\n	\n	#Ciclar indefinidamente\n	goto etiqueta\n\nend\n\n";

	//FOR ES
	codigo = "\n#Alliance - S4\n\nbegin\nvariables f, i, t\n\n	#`f`: Hasta donde contar\n	i=0\n	f=20\n	:inicio\n\n	#`i`: Iterador\n	i=i+1\n	\n	#Imprimir el contador\n	print i\n\n	#`t`: Comparador entre `i` y `f`\n	t=f-i\n	#De no ser iguales, salta a inicio\n\n	#esperar\n	io HDD1 3\n	jnz t inicio\nend\n\n";
	//FOR ES EZE AGREGA PRINT T
	//codigo = "\n#Alliance - S4\n\nbegin\nvariables f, i, t\n\n	#`f`: Hasta donde contar\n t=10\n	i=0\n	f=20\n	:inicio\n\n	#`i`: Iterador\n	i=i+1\n	\n	#Imprimir el contador\n	print i\n\n	#`t`: Comparador entre `i` y `f`\n	t=f-i\nprint t\n	#De no ser iguales, salta a inicio\n\n	#esperar\n	io HDD1 3\n	jnz t inicio\nend\n\n";

	//FIBO
	//codigo = "\nbegin\nvariables x\n	x <- fibo 8\n# Esperable: SegFault en el 10mo (40)\n\n#	x <- fibo 8\n#Esperable: 21\n\n	textPrint Solucion:\n		print x\nend\n\nfunction fibo\nprint $0\n	jz $0 return0\n	jz $0-1 return1\nvariables a, b\n	a <- fibo $0-1\n	b <- fibo $0-2\nreturn a+b\n\n:return0\nreturn 0\n\n:return1\nreturn 1\n\n";

	//FACIL
	//codigo = "\nbegin\nvariables a, b\na = 3\nb = 5\na = b + 12\nend\n";
	//FACIL IMPRIMIENDO RESULTADO
	//codigo = "\nbegin\nvariables a, b\na = 3\nb = 5\na = b + 12\nprint a\nend\n";

//	codigo = "begin\n!colas = 20\n:etiqueta\nprint !colas\n!colas = !colas - 1\njnz !colas etiqueta\nend";
	//codigo = "begin\nvariables x\nx <- fibo 8\n#Esperable: 21\n\ntextPrint Solucion:\nprint x\nend\n\nfunction fibo\nprint $0\njz $0 return0\njz $0-1 return1\nvariables a, b\na <- fibo $0-1\nb <- fibo $0-2\ntextPrint a:\nprint a\ntextPrint b:\nprint b\nreturn a+b\n\n:return0\nreturn 0\n:return1\nreturn 1#";
//	codigo = "#!/usr/bin/ansisop\nbegin\n#un vector de 5 posiciones\nvariables a, b, c, d, e, i\na=111\nb=222\nc=333\n#i va a ser mi iterador, me interesa que empieze en 0\ni=0\n\n#Bucle del for\n:for\n#imprime el valor iavo del vector\nprint *&a+i\n#pongo en 0 el valor de la posicion para verificar escritura\n*&a+i = 0\n#avanzo en el vector (de a 4 posiciones, 1 int)\ni=i+4\n#Si i no es 20 (5 posiciones del vector * 4 temanio de las variables)\njnz 20-i for\n\n#Final del bucle\ntextPrint Fin\nend\n\n";
//	codigo = "begin\nvariables x\nx <- fibo 8\n#Esperable: 21\n\ntextPrint Solucion:\nprint x\nend\n\nfunction fibo\nprint $0\njz $0 return0\njz $0-1 return1\nvariables a, b\na <- fibo $0-1\nb <- fibo $0-2\ntextPrint a:\nprint a\ntextPrint b:\nprint b\nreturn a+b\n\n:return0\nreturn 0\n:return1\nreturn 1\n";
	//codigo = "begin\nvariables x\nx <- fibo 8\n#Esperable: 21\n\ntextPrint Solucion:\nprint x\nend\n\nfunction fibo\nreturn 1#";
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
	}else{
		log_trace(trace_log,"No se pudo iniciar el programa ansisop\n");
	}

//	listen_sigint_signal();

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
				printf("El programa finalizo correctamente\n");
				log_trace(trace_log,"El programa finalizo correctamente\n");
				fflush(stdout);
			} else if (finalizar->motivo == 2){
				printf("El programa finalizo por Stack Overflow\n");
				log_trace(trace_log,"El programa finalizo por Stack Overflow\n");
				fflush(stdout);
			} else if (finalizar->motivo == 7) {
				printf("Insuficiente espacio como para cargar el programa");
				log_trace(trace_log, "Insuficiente espacio como para cargar el programa");
				fflush(stdout);
			} else if (finalizar->motivo == 58) {
				printf("No hay frames libres, se finaliza el proceso");
				log_trace(trace_log, "No hay frames libres, se finaliza el proceso");
				fflush(stdout);
			} else {
				printf("El programa no pudo finalizar correctamente\n");
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

}

//void listen_sigint_signal() {
//	if (signal(SIGINT, sigint_handler) == SIG_ERR)
//		printf("\ncan't catch SIGINT\n");
//}

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
/*
//Agrega Newton -- Inicio
void cargaArray(char* array, struct FILE* codeF) {
	int c;
	int i = 0;
	for (i = 0; EOF; i++) {
		c = getc(codeF);
		if (c == EOF) {
			array[i] = '\0';
			break;
		} else {
			array[i] = c;
		}
	}
}
void imprimeArray(const int cantCar,char array[]) {
	int i = 0;
	for (i = 0; i < cantCar; i++)
		printf("%c", array[i]);
}
//Agrega Newton -- Fin
*/
