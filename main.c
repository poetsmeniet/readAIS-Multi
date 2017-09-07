/*
readAIS-Multi

Copyright (c) 2017 Thomas Wink <thomas@geenbs.nl>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"
#include "readAIS-Multi-targetLogger.h"
#include "logger.h"
#define MAXLEN 120
#define DEVICE "/dev/ttyUSB0"
#define clear() printf("\033[H\033[J") //to clear the linux term

FILE *openDevice();
void freeLinkedList(atl *targetList);

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
    targetLog->lastUpdate = 99;
    targetLog->dst= 0;
    targetLog->next = NULL;
    
    //Country code stuff
    struct cntyCodes cc[400];
    returnCntyCodes(cc); //test efficiency, maybe run this once in main

    FILE *fp = openDevice();
    while(1){
    //while(!feof(fp)){
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
    freeLinkedList(targetLog);
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

void freeLinkedList(atl *targetList){
    atl *head = targetList;
    atl *curr;
    while ((curr = head) != NULL) { // set curr to head, stop if list empty.
        head = head->next;          // advance head to next element.
        free (curr);                // delete saved pointer.
    }
    printf("Done freeing targetList\n");
}
