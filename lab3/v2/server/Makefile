all: sender receiver objects

sender: suft.c
	gcc -o suft suft.c

receiver: suftd.c
	gcc -o suftd suftd.c

objects:
	gcc -c suft.c -o suft.o
	gcc -c suftd.c -o suftd.o

clean:
	find . -type f ! -name "*.param" ! -name "*.c" ! -name "*.nfs*" ! -name "*.h" ! -name "Makefile" -delete
