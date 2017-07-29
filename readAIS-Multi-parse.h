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
    unsigned int msgType;
    unsigned int partNo;
    float sog;
    unsigned int heading;
    float cog;
    char vesselName[40];
    unsigned int MMSI;
    float lon;
    float lat;
    unsigned int ts;
}aisP;

unsigned int nmeaChecksumVerified(char *sentence);
void printErr(char *msg);
void parseMsg(char *line, aisP *aisMsg);
void returnBinaryPayload(char *payl, aisP *aisPacket);
void assignUIntFromBin(char *bin, unsigned int *target);
float returnU1FloatFromBin(char *bin);
int assignSubstring(char *myStr, size_t start, size_t end, char *subStr);
void retShipnameFrom6bitsString(char *binString, aisP *aisPacket);
void assignLatLon(char *lonBin, char *latBin, aisP *aisPacket);
void decodePayload(aisP * aisPacket);

#endif
