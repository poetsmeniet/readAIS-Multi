#ifndef readAISMultiTargetLogger_H_
#define readAISMultiTargetLogger_H_
#include "readAIS-Multi-parse.h"

typedef struct aisTargetLog{
    char vesselName[40];
    size_t msgType;
    unsigned int MMSI;
    size_t heading;
    float cog;
    float sog;
    float lat;
    float lon;
    struct aisTargetLog *next;
}atl;

void manageTargetList(aisP *aisPacket, struct aisTargetLog *targetLog);

#endif
