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
    printf("Printing current struct: %s, %i, %s\n", aisMsg->packetType, aisMsg->fragCnt, aisMsg->payload);
}

int main(void){
    aisP aisMsg;
    char *line = (char *) malloc(sizeof(char) * MAXLEN);
    size_t len = 0;
    
    FILE *fp = openDevice();
    while(1){
        getline(&line, &len, fp);

        parseMsg(line, &aisMsg);
    
        if(strcmp(aisMsg.packetType, "!AIVDM") == 0)
            printStruct(&aisMsg);
    }

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
