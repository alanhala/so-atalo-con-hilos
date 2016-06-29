# Makefile
src=./spec/main_spec.c ./spec/pages_table_spec.c ./spec/simulacion_de_umc.c ./spec/swap_spec.c  ./Test/protocolo_test.c  ./main.c ./pages_table.c ./protocolo_swap.c ./socket.c ./swap.c
obj=./spec/main_spec.o ./spec/pages_table_spec.o ./spec/simulacion_de_umc.o ./spec/swap_spec.o  ./Test/protocolo_test.o  ./main.o ./pages_table.o ./protocolo_swap.o ./socket.o ./swap.o 
inc= 
misc=Makefile
target=swap
cflags=-w
lflags= -lcommons -lcunit -lparser-ansisop -lpthread

all: $(target)

$(target) : $(obj) $(misc)
	gcc $(cflags)  -o $(target) $(obj) $(lflags)

%.o : %.c $(misc) $(inc)
	gcc $(cflags) -c -o $@ $< 

clean:
	rm -f $(obj) $(target)
