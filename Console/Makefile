# Makefile
src=console_main.c protocoloConsola.c socket.c
obj=console_main.o protocoloConsola.o socket.o
inc= protocoloConsola.h socket.h
misc=Makefile
target=/usr/bin/ansisop
cflags=-w
lflags= -lcommons -lcunit -lparser-ansisop -lpthread

all: $(target)

$(target) : $(obj) $(misc)
	sudo gcc $(cflags)  -o $(target) $(obj) $(lflags)

%.o : %.c $(misc) $(inc)
	gcc $(cflags) -c -o $@ $< 

clean:
	sudo rm -f $(obj) $(target)
