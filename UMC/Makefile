# Makefile
src=./Test/SwapTest.c ./Test/conexionUMCTest.c ./Test/simulacion.c ./Test/test.c ./Test/tlbTest.c  ./levanta_config_files.c ./main.c ./memoriaPrincipal.c ./protocoloUMC.c ./socket.c 
obj=./Test/SwapTest.o ./Test/conexionUMCTest.o ./Test/simulacion.o ./Test/test.o ./Test/tlbTest.o  ./levanta_config_files.o ./main.o ./memoriaPrincipal.o ./protocoloUMC.o ./socket.o 
inc= 
misc=Makefile
target=umc
cflags=-w
lflags= -lcommons -lcunit -lparser-ansisop -lpthread

all: $(target)

$(target) : $(obj) $(misc)
	gcc $(cflags)  -o $(target) $(obj) $(lflags)

%.o : %.c $(misc) $(inc)
	gcc $(cflags) -c -o $@ $< 

clean:
	rm -f $(obj) $(target)
