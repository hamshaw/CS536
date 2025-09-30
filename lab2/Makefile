all: server client objects
server: fifocmds.c
	gcc -o server fifocmds.c
client: fifocmdc.c hasArguements.c pipeCloseHandler.c
	gcc -o client fifocmdc.c hasArguements.c pipeCloseHandler.c

objects:
	gcc -c fifocmds.c -o fifocmds.o
	gcc -c fifocmdc.c -o fifocmdc.o
	gcc -c hasArguements.c -o hasArguements.o
	gcc -c pipeCloseHandler.c -o pipeCloseHandler.o
clean:
	find . -type f ! -name "*.c" ! -name "*.h" ! -name "Makefile" -delete
