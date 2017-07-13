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

void printStruct(aisP *p){
    printf("(%d)\t%i\t%.2f kts\t%.2f°\t\t%.6f %.6f\tVesselName: %s\n",
        p->msgType, p->MMSI, p->sog, p->cog, p->lat, p->lon, p->vesselName);
}

int main(void){
    aisP aisPacket;
    char *line = malloc(sizeof(char) * MAXLEN);
    size_t len = 0;

    struct aisTargetLog *targetLog;
    targetLog = malloc(sizeof(struct aisTargetLog)); //Stores AIS targets
    //memcpy(targetLog->vesselName, "testme", 6);
    //targetLog->msgType = 0;
    targetLog->next = NULL;

    FILE *fp = openDevice();
    while(1){
        memcpy(aisPacket.vesselName, "Unknown\0", 8 * sizeof(char));
        getline(&line, &len, fp);
        unsigned int prevVessel = 123456789;
                
        //Get packet details into convenient struct
        parseMsg(line, &aisPacket);
    
        //Get binary payload into struct
        returnBinaryPayload(aisPacket.payload, &aisPacket);

        //printf("Length is %d\n", strlen(aisPacket.binaryPayload));
        if(strlen(aisPacket.binaryPayload) == 170){
            //Decode bitstring (binary payload0
            decodePayload(&aisPacket);

            if(aisPacket.msgType == 18\
                || aisPacket.msgType == 19\
                || aisPacket.msgType == 915\
                || aisPacket.msgType == 24\
                || aisPacket.msgType == 1\
                || aisPacket.msgType == 2\
                || aisPacket.msgType == 3\
              ){
                if(aisPacket.MMSI != prevVessel){
                    //printStruct(&aisPacket);
                    prevVessel = aisPacket.MMSI;
                }else{
                     printf("Ignoring ais packet, previous vessel was already %i\n", prevVessel);
                     //printf(".");
                }

                //pass data to manage target list
                manageTargetList(&aisPacket, targetLog);
        //printf("*** ais sentence: '%s', checksum: %x\n", line, 1); 
        printf("*** ais sentence: '%s', checksum: %x\n", line, returnNmeaChecksum(line)); 

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
