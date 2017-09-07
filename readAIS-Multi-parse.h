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
    unsigned int length;
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
