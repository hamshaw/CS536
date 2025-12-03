all: sender receiver tserver tclient objects

sender:
	$(MAKE) -C sender sender

receiver:
	$(MAKE) -C receiver receiver

tserver: tvpns.c
	gcc -o tvpns tvpns.c

tclient: tvpnc.c
	gcc -o tvpnc tvpnc.c

objects:
	gcc -c tvpns.c -o tvpns.o
	gcc -c tvpnc.c -o tvpnc.o
	gcc -c sender/suft.c -o suft.o -lm
	gcc -c receiver/suftd.c -o suftd.o -lm
	gcc -c sender/paramsProcessor.c -o paramsProcessor.o
	gcc -c receiver/writeToFile.c -o writeToFile.o
	gcc -c sender/alarmHandler.c -o alarmHandler.o
	gcc -c sender/getDroppedAcks.c -o getDroppedAcks.o
	gcc -c receiver/getDroppedPackets.c -o getDroppedPackets.o
	gcc -c math_functions.c -o math_functions.o
	gcc -c alarmHandler.c -o alarmHandler.o
clean:
	find . -type f ! -name "README" ! -name "*.c" ! -name "*.nfs*" ! -name "*.h" ! -name "Makefile" -delete
