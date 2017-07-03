#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

void parseMsg(char *line, aisP *aisMsg){
    //Extract fields and store in struct
    char *token, *str, *tofree;
    tofree = str = strdup(line);  
    size_t tokNr = 1;
    while ((token = strsep(&str, ","))){ 
        if(tokNr == 1)
            strncpy(aisMsg->packetType, token, sizeof(aisMsg->packetType));
        if(tokNr == 2)
            aisMsg->fragCnt = atoi(token);
        if(tokNr == 3)
            aisMsg->fragNr = atoi(token);
        if(tokNr == 4)
            aisMsg->seqId = atoi(token);
        if(tokNr == 5)
            aisMsg->chanCode = *token;
        if(tokNr == 6)
            strncpy(aisMsg->payload, token, sizeof(aisMsg->payload));
        if(tokNr == 7)
            aisMsg->padding = atoi(token);
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

void returnBinaryPayload(char *payl){
    int i = 0;

    //char payl[] = "B3P=Ot000?u;tTW?G0L93w`UoP06";
    //char payl[] = "test"; //works
    //char *payl = malloc(sizeof(char) * 6);
    //payl[0] = 't';
    //payl[1] = 'e';
    //payl[2] = 's';
    //payl[3] = 't';
    //payl[4] = '1';
    //payl[5] = '\0';

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

    printf("\nBinary payload (sz: %d): %s\n",strlen(concatstr) , concatstr);

    //free(concatstr); //this seems to be conflicting with out of scope de-alloc?
 
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}
