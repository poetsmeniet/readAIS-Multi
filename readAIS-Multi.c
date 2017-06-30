#include <stdio.h>
#include <stdlib.h>
#include "readAIS-Multi-parse.h"
#define MAXLEN 120
#define DEVICE "/dev/ttyUSB0"

//Read AIS-MULTI data device
//- parse AIS B type position report
//- parse ship name
//
//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

//struct aisMessage{
//    char packetType[20];
//    size_t fragCnt;
//    size_t fragNr;
//    size_t seqId;
//    char chanCode;
//    char payload[100];
//    size_t padding;
//    size_t checksum;
//};

//void parseMsg(char * line);
//void printErr(char *msg);
FILE *openDevice();

int main(void){
    char *line = (char *) malloc(sizeof(char) * MAXLEN);
    size_t len = 0;
    
    FILE *fp = openDevice();
    while(1){
        getline(&line, &len, fp);

        parseMsg(line);
    }

    return 0;
}

//void parseMsg(char * line){
//        printf("%s", line);
//        //Extract fields and store in struct
//
//}

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

//void printErr(char *msg){
//    fprintf(stdout, "ERR: %s\n", msg);
//}
