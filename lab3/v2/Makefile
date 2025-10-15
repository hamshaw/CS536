all: sender receiver objects

sender:
	gcc -o suft sender/suft.c sender/alarmHandler.c sender/paramsProcessor.c

receiver:
	gcc -o suftd receiver/suftd.c receiver/writeToFile.c

objects:
	gcc -c sender/suft.c -o suft.o
	gcc -c receiver/suftd.c -o suftd.o
	gcc -c sender/paramsProcessor.c -o paramsProcessor.o
	gcc -c receiver/writeToFile.c -o writeToFile.o
	gcc -c sender/alarmHandler.c -o alarmHandler.o
clean:
	find . -type f ! -name "*.txt" ! -name "*.param" ! -name "*.c" ! -name "*.nfs*" ! -name "*.h" ! -name "Makefile" -delete


.PHONY: sender receiver objects clean all
