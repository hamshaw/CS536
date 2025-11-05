all: server client objects
server: tvpns.c
	gcc -o tvpns tvpns.c
client: tvpnc.c 
	gcc -o tvpnc tvpnc.c

objects:
	gcc -c tvpns.c -o tvpns.o
	gcc -c tvpnc.c -o tvpnc.o
clean:
	find . -type f ! -name "README" ! -name "*.c" ! -name "*.h" ! -name "Makefile" -delete
