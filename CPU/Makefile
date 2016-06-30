# Makefile
src=cpu_main.c cpu.c primitive.c protocoloCPU.c socket.c  
obj=cpu_main.o cpu.o primitive.o protocoloCPU.o socket.o   
inc= 
misc=Makefile
target=cpu
cflags=-w
lflags= -lcommons -lparser-ansisop -lpthread

all: $(target)

$(target) : $(obj) $(misc)
	gcc $(cflags)  -o $(target) $(obj) $(lflags)

%.o : %.c $(misc) $(inc)
	gcc $(cflags) -c -o $@ $< 

clean:
	rm -f $(obj) $(target)
