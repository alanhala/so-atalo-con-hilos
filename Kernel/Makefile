# Makefile
src=./spec/kernel_spec.c  ./kernel.c ./kernel_communication.c ./levanta_config_file.c ./main.c ./protocoloKernel.c ./scheduler.c ./socket.c    
obj=./spec/kernel_spec.o  ./kernel.o ./kernel_communication.o ./levanta_config_file.o ./main.o ./protocoloKernel.o ./scheduler.o ./socket.o    
inc= 
misc=Makefile
target=kernel
cflags=-w
lflags= -lcommons -lcunit -lparser-ansisop -lpthread

all: $(target)

$(target) : $(obj) $(misc)
	gcc $(cflags)  -o $(target) $(obj) $(lflags)

%.o : %.c $(misc) $(inc)
	gcc $(cflags) -c -o $@ $< 

clean:
	rm -f $(obj) $(target)
