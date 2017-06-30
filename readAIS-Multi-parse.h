#ifndef readAISMultiparse_H_
#define readAISMultiparse_H_

struct aisMessage{
    char packetType[20];
    size_t fragCnt;
    size_t fragNr;
    size_t seqId;
    char chanCode;
    char payload[100];
    size_t padding;
    size_t checksum;
};

void printErr(char *msg);

void parseMsg(char * line);

#endif
