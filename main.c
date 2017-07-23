#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"
#include "readAIS-Multi-targetLogger.h"
#define MAXLEN 120
#define DEVICE "/dev/ttyUSB0"

//Read AIS-MULTI data device
//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

FILE *openDevice();

int main(void){
    aisP aisPacket;
    char *line = malloc(sizeof(char) * MAXLEN);
    size_t len = 0;

    //Inititalize target log struct (bookkeeping AIS targets)
    struct aisTargetLog *targetLog;
    targetLog = malloc(sizeof(struct aisTargetLog)); //Stores AIS targets
    targetLog->next = NULL;

    FILE *fp = openDevice();
    while(1){
        memcpy(aisPacket.vesselName, "Unknown\0", 8 * sizeof(char));

        getline(&line, &len, fp);
                
        parseMsg(line, &aisPacket);//Get packet tokens
    
        returnBinaryPayload(aisPacket.payload, &aisPacket); //Get binary payload
        
        if(strlen(aisPacket.binaryPayload) > 160){

            //Decode bitstring (binary payload0
            decodePayload(&aisPacket);

            if(aisPacket.msgType == 18\
                || aisPacket.msgType == 19\
                || aisPacket.msgType == 5\
                || aisPacket.msgType == 24\
                || aisPacket.msgType == 1\
                || aisPacket.msgType == 2\
                || aisPacket.msgType == 3\
              ){
                
              manageTargetList(&aisPacket, targetLog);
            }
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
