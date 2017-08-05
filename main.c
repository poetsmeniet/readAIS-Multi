#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"
#include "readAIS-Multi-targetLogger.h"
#include "logger.h"
#define MAXLEN 120
//#define DEVICE "/dev/ttyUSB0"
#define DEVICE "dump2"
#define clear() printf("\033[H\033[J") //to clear the linux term

//Read AIS-MULTI data device
//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

FILE *openDevice();

int main(void){
    char logFile[] = "aisMulti.log";
    initLogr(logFile);
    aisP aisPacket;
    char *line = malloc(sizeof(char) * MAXLEN);
    size_t len = 0;

    //Inititalize target log struct (bookkeeping AIS targets)
    struct aisTargetLog *targetLog;
    targetLog = malloc(sizeof(struct aisTargetLog)); //Stores AIS targets
    targetLog->msgType = 99;
    targetLog->next = NULL;
    
    //Country code stuff
    struct cntyCodes cc[400];
    returnCntyCodes(cc); //test efficiency, maybe run this once in main

    FILE *fp = openDevice();
    //while(1){
    while(!feof(fp)){
        getline(&line, &len, fp);
        if(strlen(line) > 35){
            memcpy(aisPacket.vesselName, "Unknown\0", 8 * sizeof(char));
                    
            parseMsg(line, &aisPacket);//Get packet tokens
    
            returnBinaryPayload(aisPacket.payload, &aisPacket); //Get binary payload
            
            decodePayload(&aisPacket);//Decode bitstring (binary payload)
            //printf(" and msgType is: %d\n", aisPacket.msgType);
            if(aisPacket.msgType == 18\
                || aisPacket.msgType == 19\
                || aisPacket.msgType == 5\
                || aisPacket.msgType == 24\
                || aisPacket.msgType == 1\
                || aisPacket.msgType == 2\
                || aisPacket.msgType == 3\
              ){
             manageTargetList(&aisPacket, targetLog, cc);
            }
            logr(0, "msgType %d detected (%d):: %s", aisPacket.msgType, aisPacket.MMSI, aisPacket.vesselName);
        }
        printf(".");
        line[0]='\0';
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
