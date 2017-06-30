#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

void parseMsg(char * line, aisP aisMsg){
    printf("%s", line);
    //Extract fields and store in struct
    
    char *token, *str, *tofree;
    tofree = str = strdup(line);  // We own str's memory now.
    size_t tokNr = 1;
    while ((token = strsep(&str, ","))){ 
        printf("%s - ",token);
        if(tokNr == 1)
            strncpy(aisMsg.packetType, token, sizeof(aisMsg.packetType));
    }
    free(tofree);
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}
