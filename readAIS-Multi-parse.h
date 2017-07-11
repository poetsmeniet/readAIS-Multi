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
    char binaryPayload[500];
    size_t msgType;
    float sog;
    size_t heading;
    float cog;
    char vesselName[40];
    unsigned int MMSI;
    float lon;
    float lat;
    unsigned int ts;
}aisP;

void printErr(char *msg);

void parseMsg(char *line, aisP *aisMsg);

void returnBinaryPayload(char *payl, aisP *aisPacket);

unsigned int returnUIntFromBin(char *bin);
float returnU1FloatFromBin(char *bin);
float COGtmp_returnU1FloatFromBin(char *bin);

int retSubstring(char *myStr, size_t start, size_t end, char *subStr);
void returnAsciiFrom6bits(char *binString, aisP *aisPacket);
void returnLatLon(char *lonBin, char *latBin, aisP *aisPacket);
#endif
