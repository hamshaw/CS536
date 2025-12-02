all: pserver pclient tserver tclient objects

pserver: udppings.c
	gcc -o udppings udppings.c

pclient: udppingc.c math_functions.c alarmHandler.c
	gcc -o udppingc udppingc.c math_functions.c alarmHandler.c -lm

tserver: tvpns.c
	gcc -o tvpns tvpns.c

tclient: tvpnc.c
	gcc -o tvpnc tvpnc.c

objects:
	gcc -c tvpns.c -o tvpns.o
	gcc -c tvpnc.c -o tvpnc.o
	gcc -c udppingc.c -o udppingc.o
	gcc -c udppings.c -o udppingc.o
	gcc -c math_functions.c -o math_functions.o
	gcc -c alarmHandler.c -o alarmHandler.o
clean:
	find . -type f ! -name "README" ! -name "*.c" ! -name "*.nfs*" ! -name "*.h" ! -name "Makefile" -delete
