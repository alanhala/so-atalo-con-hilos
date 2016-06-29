# Makefile
src=cpu_main.c conexionCPUTest.c cpu.c  primitive.c protocoloCPU.c socket.c tests.c   
obj=cpu_main.o conexionCPUTest.o cpu.o  primitive.o protocoloCPU.o socket.o tests.o   
inc= 
misc=Makefile
target=cpu
cflags=-w
lflags= -lcommons -lcunit -lparser-ansisop -lpthread

all: $(target)

$(target) : $(obj) $(misc)
	gcc $(cflags)  -o $(target) $(obj) $(lflags)

%.o : %.c $(misc) $(inc)
	gcc $(cflags) -c -o $@ $< 

clean:
	rm -f $(obj) $(target)
