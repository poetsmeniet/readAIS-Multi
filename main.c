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

void printStruct(aisP *aisPacket){
    printf("Printing current struct: (%c) %s, (%i of %i), %s - padding: %i\n", aisPacket->chanCode, aisPacket->packetType, aisPacket->fragNr, aisPacket->fragCnt, aisPacket->payload, aisPacket->padding);
}

int main(void){
    aisP aisPacket;
    char *line = (char *) malloc(sizeof(char) * MAXLEN);
    size_t len = 0;
    
    FILE *fp = openDevice();
    while(1){
        getline(&line, &len, fp);
        //Get packet details into convenient struct
        parseMsg(line, &aisPacket);
    
        //Debug print
        if(
                aisPacket.fragCnt == 1 || aisPacket.payload[0] == '1'\
                && (aisPacket.fragCnt == 1 || aisPacket.payload[0]) == '2'\
                && (aisPacket.fragCnt == 1 || aisPacket.payload[0]) == '3'\
                && (aisPacket.fragCnt == 1 || aisPacket.payload[0]) == 'B'\
                && (aisPacket.fragCnt == 1 || aisPacket.payload[0]) == 'C'\
                ){
            printf("Whole msg: %s\n", line);
            printStruct(&aisPacket);

            //Get binary payload
            returnBinaryPayload(aisPacket.payload);
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
