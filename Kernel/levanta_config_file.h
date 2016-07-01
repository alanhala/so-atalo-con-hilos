#include <commons/config.h>
#include <commons/log.h>
#include "kernel.h"

#define EVENT_SIZE (sizeof (struct inotify_event) + 24)
#define BUF_LEN (1024 * EVENT_SIZE)

typedef struct kernelConfigFile {
	uint32_t quantum,
			quantum_sleep,
			stack_size,
			backlog;
	char 	**idDispositivo,
			**sleepIO,
			**idSemaforo,
			**inicioSemaforo,
			**globalVar;
	char	*umc_ip,
			*puerto_programa,
			*puerto_cpu,
			*server_ip;
} KernelConfigFile;



void *cargar_configuracion(t_kernel *kernel);
void levantaConfigFileEnVariables(KernelConfigFile *ptrvaloresConfigFile,t_config *ptrConfig);
void liberaVariables(t_log* trace_log, t_config* ptrConfig, t_config* ptrConfigUpdate);
void cargar_kernel(KernelConfigFile *config, t_kernel *kernel);
void detectaCambiosEnConfigFile();
unsigned leerUnsigned(t_config *config, char* key);
char** leerArray(t_config *config, char* key);
char* leer_string(t_config *config, char* key);
