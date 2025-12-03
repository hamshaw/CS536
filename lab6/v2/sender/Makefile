all: sender

sender:
	gcc -o suft suft.c getDroppedAcks.c alarmHandler.c paramsProcessor.c -lm

objects:
	gcc -c sender/suft.c -o suft.o -lm
	gcc -c receiver/suftd.c -o suftd.o -lm
	gcc -c sender/paramsProcessor.c -o paramsProcessor.o
	gcc -c receiver/writeToFile.c -o writeToFile.o
	gcc -c sender/alarmHandler.c -o alarmHandler.o
	gcc -c sender/getDroppedAcks.c -o getDroppedAcks.o
	gcc -c receiver/getDroppedPackets.c -o getDroppedPackets.o
clean:
	find . -type f ! -name "*.lossmodel" ! -name "*.txt" ! -name "*.param" ! -name "*.c" ! -name "*.nfs*" ! -name "*.h" ! -name "Makefile" -delete


.PHONY: sender receiver objects clean all
