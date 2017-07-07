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

//Return power of unsigned integer
unsigned int power(unsigned int base, unsigned int exp){
    unsigned int result = 1;
    unsigned int term = base;
    while (exp){
        if (exp & 1)
            result *= term;
        term *= term;
        exp = exp >> 1;
    }
    return result;
}

unsigned int returnUIntFromBin(char *bin){
    int sz = strlen(bin);

    int cnt = 0;
    int cntRev = sz - 1;
    int decimal = 0;
    for(cnt = 0; cnt < sz; cnt++){
        if(bin[cnt] == '1'){
            decimal += power(2, cntRev);
        }
        cntRev--;
    }
    return decimal;
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

//Returns substring using given index
int retSubstring(char *myStr, size_t start, size_t end, char *subStr){
    if(end > start){
        char *s = &myStr[start];
        char *e = &myStr[end + 1];

        memcpy(subStr, s, e - s);
        return 0;
    }else{
        printf("retSubstring error: end is greater than start.\n");
        return 1;
    }
}

void returnBinaryPayload(char *payl, aisP *aisPacket){
    int i = 0;

    size_t paylSz = strlen(payl);
    printf("Payload '%s' has %i chars\n", payl, paylSz);

    char *concatstr = (char *) malloc(paylSz  * 7 * sizeof(char) + 1);
    
    while(payl[i] != '\0'){
        //To recover (de-armor) the six bits, subtract 48 from the ASCII character value; if the result is greater than 40 subtract 8
        int res1 = (payl[i] - 48);
        if(res1 > 40)
            res1 -= 8;

        //Return 6 bit ascii value
        char sixbits[6];
        ret6bit(res1, sixbits);

        //add to conactenated string
        strncat(concatstr, sixbits, (sizeof(sixbits)));

        i++;
    }

    strncpy(aisPacket->binaryPayload, concatstr, strlen(concatstr) + 1);

    //free(concatstr); //this seems to be conflicting with out of scope de-alloc?
 
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}
