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
    time_t lastUpdate;
    char cnty[3];
    struct aisTargetLog *next;
}atl;

struct cntyCodes{
    unsigned int code;
    char name[40];
    char abbrev[2];
};

void manageTargetList(aisP *aisPacket, struct aisTargetLog *targetLog, struct cntyCodes *cc);

void returnCntyCodes(struct cntyCodes *cc);
#endif
