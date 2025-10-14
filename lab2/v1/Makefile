all: server client objects
server: tcpcmds.c
	gcc -o server tcpcmds.c
client: tcpcmdc.c
	gcc -o client tcpcmdc.c

objects:
	gcc -c tcpcmds.c -o tcpcmds.o
	gcc -c tcpcmdc.c -o tcpcmdc.o
clean:
	find . -type f ! -name "*.c" ! -name "*.h" ! -name "Makefile" -delete
