#ifndef readAISMultiparse_H_
#define readAISMultiparse_H_

typedef struct aisPacket{
    char packetType[20];
    size_t fragCnt;
    size_t fragNr;
    size_t seqId;
    char chanCode;
    char payload[100];
    size_t padding;
    size_t checksum;
}aisP;

void printErr(char *msg);

void parseMsg(char * line, aisP *aisMsg);

#endif
