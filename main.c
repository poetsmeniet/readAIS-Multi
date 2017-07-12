#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"
#define MAXLEN 120
#define DEVICE "/dev/ttyUSB0"

//Read AIS-MULTI data device
//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

FILE *openDevice();

void printStructVerbose(aisP *p){
    printf("\nVesselName: %s\n- msgType: %d\n- MMSI: %i\n- heading: %d\n- SOG: %f\n- COG: %.2f\n- Lon: %.6f\n- Lat: %.6f\n",\
            p->vesselName, p->msgType, p->MMSI, p->heading, p->sog, p->cog, p->lon, p->lat);
    printf("- googMapLink: https://maps.google.com/maps?f=q&q=%.6f,%.6f&z=16\n", p->lon, p->lat);
}

void printStruct(aisP *p){
    printf("(%d)\t%i\t%.2f kts\t%.2f°\t\t%.6f %.6f\tVesselName: %s\n",
        p->msgType, p->MMSI, p->sog, p->cog, p->lat, p->lon, p->vesselName);
}

int main(void){
    aisP aisPacket;
    char *line = malloc(sizeof(char) * MAXLEN);
    size_t len = 0;
    unsigned int prevVessel = 123456789;
    size_t start = 0;
    size_t end = 5;
    char *subStr;
    
    FILE *fp = openDevice();
    while(1){
       memcpy(aisPacket.vesselName, "Unknown\0", 8 * sizeof(char));
       getline(&line, &len, fp);
       
       //Get packet details into convenient struct
       parseMsg(line, &aisPacket);
    
       //Get binary payload into struct
       returnBinaryPayload(aisPacket.payload, &aisPacket);

       // *** this needs to be generalized, now only coding for type 18 (class B)
       // 1. Rewrite all function calls to assign values in function call
       // 2. Move subString calls to applicable functions
       // 3. Type 24 msgs: call collect data function
       // variables:
       // - msgType (determines offset diffs)
       // - start, end
       // - function to call
       // - aisPacket item
       //
       //get message type
       start = 0;
       end = 5;
       subStr = malloc((end - start) + 2 * sizeof(char));
       retSubstring(aisPacket.binaryPayload, start, end, subStr);
       aisPacket.msgType= returnUIntFromBin(subStr);
       free(subStr);
        
       //get true heading
       start = 124;
       end = 132;
       subStr = malloc((end - start) + 2 * sizeof(char));
       retSubstring(aisPacket.binaryPayload, start, end, subStr);
       aisPacket.heading = returnUIntFromBin(subStr);
       free(subStr);
       
       //MMSI at offset 8-37, and convert to decimal
       start = 8;
       end = 37;
       subStr = malloc((end - start) + 2 * sizeof(char));
       retSubstring(aisPacket.binaryPayload, start, end, subStr);
       aisPacket.MMSI = returnUIntFromBin(subStr);
       free(subStr);

       //B
       //lon: 57-84
       //lat: 85-111
       //
       if(aisPacket.msgType == 18\
               || aisPacket.msgType == 19){
           
           //get lon
           start = 57;
           end = 84;
           char *subStrLon = malloc((end - start) + 3 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStrLon);
           //getlat 
           start = 85;
           end = 111;
           char *subStrLat = malloc((end - start) + 2 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStrLat);
           //set lat/lon
           returnLatLon(subStrLon, subStrLat, &aisPacket);
           free(subStrLon);
           free(subStrLat);

           //get speed over ground (std class b  CS position report
           start = 46;
           end = 55;
           subStr = malloc((end - start) + 2 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStr);
           aisPacket.sog= returnU1FloatFromBin(subStr);
           free(subStr);
           
           //get cog class b
           start = 112;
           end = 123;
           subStr = malloc((end - start) + 2 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStr);
           aisPacket.cog = COGtmp_returnU1FloatFromBin(subStr);
           free(subStr);
           
           //get timestamp
           start = 133;
           end = 138;
           subStr = malloc((end - start) + 3 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStr);
           aisPacket.ts = returnUIntFromBin(subStr);
           free(subStr);
           
           
       }else if(aisPacket.msgType == 1\
               || aisPacket.msgType == 2\
               || aisPacket.msgType == 3\
               ){
           //get speed over ground class A
           start = 50;
           end = 59;
           subStr = malloc((end - start) + 2 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStr);
           aisPacket.sog= returnU1FloatFromBin(subStr);
           free(subStr);

           //get cog class a
           start = 116;
           end = 127;
           subStr = malloc((end - start) + 2 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStr);
           aisPacket.cog = COGtmp_returnU1FloatFromBin(subStr);
           free(subStr);

           //A
           //lon: 61-88
           //lat: 89-115
           //get lon
           start = 61;
           end = 88;
           char *subStrLon = malloc((end - start) + 3 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStrLon);
           //getlat 
           start = 89;
           end = 115;
           char *subStrLat = malloc((end - start) + 2 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStrLat);
           //set lat/lon
           returnLatLon(subStrLon, subStrLat, &aisPacket);
           free(subStrLon);
           free(subStrLat);
       }
           
       //addendum to protocol for ais B transponders, integrate later to add to struct
       if(aisPacket.msgType == 24){
           //get ship name and more from type 24 message, correlation through MMSI
           start = 40;
           end = 159;
           subStr = malloc((end - start) + 2 * sizeof(char));
           retSubstring(aisPacket.binaryPayload, start, end, subStr);
           returnAsciiFrom6bits(subStr, &aisPacket);
           free(subStr);

           //get cog class a, msgtype 24 (does not contain this info)
           aisPacket.cog = 0.0;
           
           //type 24, class B does not have lat/lon.. overwriting with 0 fr debug prints
           aisPacket.sog = 0;
           aisPacket.lon = 0;
           aisPacket.lat = 0;
       }

       if(aisPacket.msgType == 18\
           || aisPacket.msgType == 9119\
           || aisPacket.msgType == 915\
           || aisPacket.msgType == 24\
           || aisPacket.msgType == 1\
           || aisPacket.msgType == 2\
           || aisPacket.msgType == 3\
         ){
           if(aisPacket.MMSI != prevVessel){
                printStruct(&aisPacket);
                prevVessel = aisPacket.MMSI;
           }else{
                //printf("Ignoring ais packet, previous vessel was already %i\n", prevVessel);
                //printf(".");
           }
       }

    }
    free(line);
    return 0;
}

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
