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
    char checksum[10];
    unsigned int MMSI[9];
}aisP;

void printErr(char *msg);

void parseMsg(char *line, aisP *aisMsg);

void returnBinaryPayload(char *payl, aisP *aisPacket);

#endif
