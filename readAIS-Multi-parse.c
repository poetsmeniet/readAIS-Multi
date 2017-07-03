#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

void parseMsg(char *line, aisP *aisPacket){
    //Extract fields and store in struct
    char *token, *str, *tofree;
    tofree = str = strdup(line);  
    size_t tokNr = 1;
    while ((token = strsep(&str, ","))){ 
        if(tokNr == 1)
            strncpy(aisPacket->packetType, token, sizeof(aisPacket->packetType));
        if(tokNr == 2)
            aisPacket->fragCnt = atoi(token);
        if(tokNr == 3)
            aisPacket->fragNr = atoi(token);
        if(tokNr == 4)
            aisPacket->seqId = atoi(token);
        if(tokNr == 5)
            aisPacket->chanCode = *token;
        if(tokNr == 6)
            strncpy(aisPacket->payload, token, sizeof(aisPacket->payload));
        if(tokNr == 7)
            aisPacket->padding = atoi(token);
        tokNr++;
    }
    free(tofree);
}

void ret6bit(char myChar, char *sixbits){
    int i;
    size_t bit = 0;
    int cnt = 5;
    for (i = 0; i < 6; i++) {
        bit = myChar >> i & 1;
        sixbits[cnt] = bit + '0';
        cnt--;
    }
        sixbits[6] = '\0';
}

void returnBinaryPayload(char *payl, aisP *aisPacket){
    int i = 0;


    //size_t paylSz = ((sizeof(payl) * sizeof(char)) - 1);
    size_t paylSz = strlen(payl);
    printf("Payload '%s' has %i chars\n", payl, paylSz);

    char *concatstr = (char *) malloc(paylSz  * 7 * sizeof(char) + 1);
    
    while(payl[i] != '\0'){
        //To recover (de-armor) the six bits, subtract 48 from the ASCII character value; if the result is greater than 40 subtract 8
        int res1 = (payl[i] - 48);
        if(res1 > 40)
            res1 -= 8;

        char sixbits[6];
        ret6bit(res1, sixbits);
        //add to conactenated string
        strncat(concatstr, sixbits, (sizeof(sixbits)));
        //printf("'%c' = %i - 48 = %i \t :: bits: %s\n", payl[i], payl[i], res1, sixbits);

        i++;
    }

    printf("Binary payload (sz: %d): %s\n\n",strlen(concatstr) , concatstr);

    //free(concatstr); //this seems to be conflicting with out of scope de-alloc?
 
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}
