all: sender receiver objects

sender:
	gcc -o suft sender/suft.c sender/paramsProcessor.c

receiver:
	gcc -o suftd receiver/suftd.c

objects:
	gcc -c sender/suft.c -o suft.o
	gcc -c receiver/suftd.c -o suftd.o
	gcc -c sender/paramsProcessor.c -o paramsProcessor.o

clean:
	find . -type f ! -name "*.param" ! -name "*.c" ! -name "*.nfs*" ! -name "*.h" ! -name "Makefile" -delete


.PHONY: sender receiver objects clean all
