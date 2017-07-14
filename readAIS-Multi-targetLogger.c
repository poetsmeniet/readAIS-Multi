#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-targetLogger.h"
#include "readAIS-Multi-parse.h"

void printTargetList(struct aisTargetLog *targetLog){
    atl *alist = targetLog;
    printf("\n");
    while(alist->next != NULL){
        printf("-(%d)\t%i\t%.2f kts\t%.2f°\t\t%.6f %.6f\tVesselName: %s\n",\
            alist->msgType, alist->MMSI, alist->sog, alist->cog, alist->lat, alist->lon, alist->vesselName);
        alist = alist->next;
    }
}

void pushTarget(struct aisTargetLog *targetLog, aisP *aisPacket){
    atl *pushList = targetLog;
    while(pushList->next != NULL){
        pushList = pushList->next;
    }

    pushList->next = malloc(sizeof(struct aisTargetLog));
    memcpy(pushList->vesselName, aisPacket->vesselName, sizeof(aisPacket->vesselName));
    pushList->msgType = aisPacket->msgType;
    pushList->MMSI = aisPacket->MMSI;
    pushList->heading = aisPacket->heading;
    pushList->cog = aisPacket->cog;
    pushList->sog = aisPacket->sog;
    pushList->lat = aisPacket->lat;
    pushList->lon = aisPacket->lon;
    pushList->next->next = NULL;
}

_Bool isNewTarget(atl *targetLog, aisP * aisPacket){
    atl *alist = targetLog;
    while(alist->next != NULL){
        if(alist->MMSI == aisPacket->MMSI){
            return 0;
        }
        alist = alist->next;
    }
    return 1;
}

void updateVesselName(atl *targetLog, aisP * aisPacket){
    atl *alist = targetLog;
    while(alist->next != NULL){
        if(alist->MMSI == aisPacket->MMSI){
            memcpy(alist->vesselName, aisPacket->vesselName, sizeof(aisPacket->vesselName));
            break;
        }
        alist = alist->next;
    }
}

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations
void manageTargetList(aisP *aisPacket, struct aisTargetLog *targetLog){
    if(isNewTarget(targetLog, aisPacket) && aisPacket->msgType != 24)
        pushTarget(targetLog, aisPacket);

    //added check on padding, need to implement padding changes (field 7 nmea sentence)
    if(aisPacket->msgType == 24 && aisPacket->padding == 2\
            || aisPacket->msgType == 5 && aisPacket->padding == 2)
        updateVesselName(targetLog, aisPacket);

    printTargetList(targetLog);
}

