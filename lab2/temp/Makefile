all: server client objects

server: udppings.c
	gcc -o server udppings.c

client: udppingc.c math_functions.c alarmHandler.c
	gcc -o client udppingc.c math_functions.c alarmHandler.c -lm

objects:
	gcc -c udppingc.c -o udppingc.o
	gcc -c udppings.c -o udppingc.o
	gcc -c math_functions.c -o math_functions.o
	gcc -c alarmHandler.c -o alarmHandler.o
clean:
	find . -type f ! -name "*.c" ! -name "*.nfs*" ! -name "*.h" ! -name "Makefile" -delete
