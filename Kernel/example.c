#include <parser/metadata_program.h>
#include <stdio.h>

static const char* example_ansisop_code = "#!/usr/bin/ansisop\nbegin\nvariables a,g\na = 1\ng <­ doble a\nprint g\nend\nfunction doble\nvariables f\nf = $0 + $0\nreturn f\nend\0";

int main(int argc, char **argv) {
	t_metadata_program* metadata = metadata_desde_literal(example_ansisop_code);
	t_intructions* instructions = metadata->instrucciones_serializado;
	char* etiquetas = metadata->etiquetas;
	printf("================\n");
}
