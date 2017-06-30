#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

void parseMsg(char * line, aisP *aisMsg){
    //Extract fields and store in struct
    
    char *token, *str, *tofree;
    tofree = str = strdup(line);  // We own str's memory now.
    size_t tokNr = 1;
    while ((token = strsep(&str, ","))){ 
        if(tokNr == 1)
            strncpy(aisMsg->packetType, token, sizeof(aisMsg->packetType));
        //if(tokNr == 2){
        //    size_t test_t = (size_t) token;
        //    aisMsg->fragCnt = test_t;
        //}
        //if(tokNr == 3)
        //    strncpy(aisMsg->fragNr, token, sizeof(aisMsg->fragNr));
        //if(tokNr == 4)
        //    strncpy(aisMsg->seqId, token, sizeof(aisMsg->seqId));
        //if(tokNr == 5)
        //    strncpy(aisMsg->chanCode, token, sizeof(aisMsg->chanCode));
        if(tokNr == 6)
            strncpy(aisMsg->payload, token, sizeof(aisMsg->payload));
        //if(tokNr == 7)
        //    strncpy(aisMsg->checksum, token, sizeof(aisMsg->checksum));
        tokNr++;
    }
    free(tofree);
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}
