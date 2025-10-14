all: server client objects
server: tcpcmds.c
	gcc -o server tcpcmds.c
client: tcpcmdc.c alarmHandler.c
	gcc -o client tcpcmdc.c alarmHandler.c

objects:
	gcc -c tcpcmds.c -o tcpcmds.o
	gcc -c tcpcmdc.c -o tcpcmdc.o
	gcc -c alarmHandler.c -o alarmHandler.o
clean:
	find . -type f ! -name "*.c" ! -name "*.h" ! -name "Makefile" -delete
