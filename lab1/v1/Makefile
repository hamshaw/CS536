mysh: mysh.c myinthandler.c  setupArgv.c endEverything.c helperFunctions.h
	gcc -o mysh1 mysh.c myinthandler.c setupArgv.c endEverything.c
	gcc -c mysh.c -o mysh.o
	gcc -c myinthandler.c -o myinthandler.o
	gcc -c setupArgv.c -o setupArgv.o
	gcc -c endEverything.c -o endEverything.o
clean:
	find . -type f ! -name "*.c" ! -name "*.h" ! -name "Makefile" -delete
