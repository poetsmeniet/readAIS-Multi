#include <stdio.h>
#include <stdlib.h>
#include "readAIS-Multi-parse.h"

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

void parseMsg(char * line){
        printf("%s", line);
        //Extract fields and store in struct
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}
