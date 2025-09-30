#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helperFunctions.h"
int hasArguements(char *string){
        int i = 0;
        while (i++<strlen(string)){
                if (string[i] == ' ')
                       return 1;
        }
        return 0;
}
