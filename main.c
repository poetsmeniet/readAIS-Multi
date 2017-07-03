#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"
#define MAXLEN 120
#define DEVICE "/dev/ttyUSB0"

//Read AIS-MULTI data device
//- parse AIS B type position report
//- parse ship name
//
//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

FILE *openDevice();

void printStruct(aisP *aisMsg){
    printf("Printing current struct: (%c) %s, (%i of %i), %s - padding: %i\n", aisMsg->chanCode, aisMsg->packetType, aisMsg->fragNr, aisMsg->fragCnt, aisMsg->payload, aisMsg->padding);
}

int main(void){
    aisP aisMsg;
    char *line = (char *) malloc(sizeof(char) * MAXLEN);
    size_t len = 0;
    
    FILE *fp = openDevice();
    while(1){
        getline(&line, &len, fp);
        sleep(1);
        //Get packet details into convenient struct
        parseMsg(line, &aisMsg);
    
        //Debug print
        if(strcmp(aisMsg.packetType, "!AIVDM") == 0\
                && aisMsg.fragCnt == 1){
            printStruct(&aisMsg);
            //Get binary payload
            returnBinaryPayload(aisMsg.payload);
        }
    }
    free(line);
    return 0;
}

FILE *openDevice(){
    printf("Opening device '%s'..", DEVICE);
    FILE *fp = fopen(DEVICE, "r");
    if(fp == NULL){
        printErr("Unable to open device");
        exit(1);
    }else{
        printf(" success\n");
    }
    return fp;
}
