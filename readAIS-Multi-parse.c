#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

void parseMsg(char * line, aisP *aisMsg){
    //Extract fields and store in struct
    char *token, *str, *tofree;
    tofree = str = strdup(line);  
    size_t tokNr = 1;
    while ((token = strsep(&str, ","))){ 
        if(tokNr == 1)
            strncpy(aisMsg->packetType, token, sizeof(aisMsg->packetType));
        if(tokNr == 2)
            aisMsg->fragCnt = atoi(token);
        if(tokNr == 3)
            aisMsg->fragNr = atoi(token);
        if(tokNr == 4)
            aisMsg->seqId= atoi(token);
        if(tokNr == 5)
            aisMsg->chanCode= atoi(token);
        if(tokNr == 6)
            strncpy(aisMsg->payload, token, sizeof(aisMsg->payload));
        if(tokNr == 7)
            strncpy(aisMsg->checksum, token, sizeof(aisMsg->checksum));
        tokNr++;
    }
    free(tofree);
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}
