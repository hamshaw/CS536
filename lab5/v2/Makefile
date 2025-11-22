all: server client objects
server: coolmuss.c
	gcc -o coolmuss coolmuss.c -lm 
client: coolmusc.c 
	gcc -o coolmusc coolmusc.c load_params.c muaudio.c  -lasound

objects:
	gcc -c coolmuss.c -o coolmuss.o
	gcc -c coolmusc.c -o coolmusc.o
clean:
	find . -type f ! -name "*.dat" ! -name "*.au" ! -name "README" ! -name "*.c" ! -name "*.h" ! -name "Makefile" -delete
