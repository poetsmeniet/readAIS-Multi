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

void printStruct(aisP *p){
    printf("Printing current struct: (%c) %s, (%i of %i), %s - padding: %i\n- msgType: %d\n- MMSI: %i\n- heading: %d\n- SOG: %f (%i) head: %d cog: %.2f\n- COG: %.2f\n\n", p->chanCode, p->packetType, p->fragNr, p->fragCnt, p->payload, p->padding, p->msgType, p->MMSI, p->heading, p->sog, p->MMSI, p->heading, p->cog, p->cog);
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
        //if(
        //        aisPacket.fragCnt == 1 && aisPacket.payload[0] == '1'\
        //        || (aisPacket.fragCnt == 1 && aisPacket.payload[0]) == '2'\
        //        || (aisPacket.fragCnt == 1 && aisPacket.payload[0]) == '3'\
        //        || (aisPacket.fragCnt == 1 && aisPacket.payload[0]) == 'B'\
        //        || (aisPacket.fragCnt == 1 && aisPacket.payload[0]) == 'C'\
        //        || (aisPacket.fragCnt == 1 && aisPacket.payload[0]) == 'D'\
        //        )
        //{

            //Get binary payload into struct
            returnBinaryPayload(aisPacket.payload, &aisPacket);


            // *** this needs to be generalized, now only coding for type 18 (class B)
            //get true heading
            size_t start = 124;
            size_t end = 132;
            char *subStr2 = (char *) malloc(sizeof(char) * (end - start));
            retSubstring(aisPacket.binaryPayload, start, end, subStr2);
            aisPacket.heading = returnUIntFromBin(subStr2);
            //free(subStr2);
            
            //get binary payload for MMSI at offset 8-37, and convert to decimal
            start = 8;
            end = 37;
            char *subStr = (char *) malloc(sizeof(char) * (end - start));
            retSubstring(aisPacket.binaryPayload, start, end, subStr);
            aisPacket.MMSI = returnUIntFromBin(subStr);
            //free(subStr);


            //get message type
            start = 0;
            end = 5;
            char *subStr3 = (char *) malloc(sizeof(char) * (end - start));
            retSubstring(aisPacket.binaryPayload, start, end, subStr3);
            aisPacket.msgType= returnUIntFromBin(subStr3);
            //free(subStr3);

            if(aisPacket.msgType == 18\
                    || aisPacket.msgType == 19){
                //get speed over ground (std class b  CS position report
                start = 46;
                end = 55;
                char *subStr4 = (char *) malloc(sizeof(char) * (end - start));
                retSubstring(aisPacket.binaryPayload, start, end, subStr4);
                aisPacket.sog= returnU1FloatFromBin(subStr4);
                //free(subStr4);
                
                //get cog class b
                size_t start = 112;
                size_t end = 123;
                char *subStr5 = (char *) malloc(sizeof(char) * (end - start));
                retSubstring(aisPacket.binaryPayload, start, end, subStr5);
                aisPacket.cog= COGtmp_returnU1FloatFromBin(subStr5);
                //free(subStr5);
            }else if(aisPacket.msgType == 1\
                    || aisPacket.msgType == 2\
                    || aisPacket.msgType == 3\
                    ){
                //get speed over ground (std class b  CS position report
                start = 50;
                end = 59;
                char *subStr4 = (char *) malloc(sizeof(char) * (end - start));
                retSubstring(aisPacket.binaryPayload, start, end, subStr4);
                aisPacket.sog= returnU1FloatFromBin(subStr4);
                //free(subStr4);

            }else{
                aisPacket.sog = 0.66666;
            }

            if(aisPacket.msgType == 18\
                || aisPacket.msgType == 19\
                || aisPacket.msgType == 91\
                || aisPacket.msgType == 92\
                || aisPacket.msgType == 93\
              ){
                printStruct(&aisPacket);
            }
        //}
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
