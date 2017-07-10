#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-parse.h"

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

struct sixbitAsciiTable{
    char bits[7];
    char ascii;
};

struct sixbitAsciiTable sixbitAscii[64] = {
    {"000000", ' '},{"000001", 'A'},{"000010", 'B'},
    {"000011", 'C'},{"000100", 'D'},{"000101", 'E'},
    {"000110", 'F'},{"000111", 'G'},{"001000", 'H'},
    {"001001", 'I'},{"001010", 'J'},
    {"001011", 'K'},{"001100", 'L'},{"001101", 'M'},
    {"001110", 'N'},{"001111", 'O'},{"010000", 'P'},
    {"010001", 'Q'},{"010010", 'R'},{"010011", 'S'},
    {"010100", 'T'},{"010101", 'U'},{"010110", 'V'},
    {"010111", 'W'},{"011000", 'X'},{"011001", 'Y'},
    {"011010", 'Z'},{"011011", '['},{"011100", '\\'},
    {"011101", ']'},{"011110", '^'},{"011111", '_'},
    {"100000", ' '},{"100001", '!'},{"100010", '"'},
    {"100011", '#'},{"100100", '$'},{"100101", '%'},
    {"100110", '&'},{"100111", '/'},{"101000", '('},
    {"101001", ')'},{"101010", '*'},{"101011", '+'},
    {"101100", ','},{"101101", '-'},{"101110", '.'},
    {"101111", '/'},{"110000", '0'},{"110001", '1'},
    {"110010", '2'},{"110011", '3'},{"110100", '4'},
    {"110101", '5'},{"110110", '6'},{"110111", '7'},
    {"111000", '8'},{"111001", '9'},{"111010", ':'},
    {"111011", ';'},{"111100", '<'},{"111101", '='},
    {"111110", '>'},{"111111", '?'}
};

void parseMsg(char *line, aisP *aisPacket){
    //Extract fields and store in struct
    char *token, *str, *tofree;
    tofree = str = strdup(line);  
    size_t tokNr = 1;
    while ((token = strsep(&str, ","))){ 
        if(tokNr == 1)
            memcpy(aisPacket->packetType, token, sizeof(aisPacket->packetType));
        if(tokNr == 2)
            aisPacket->fragCnt = atoi(token);
        if(tokNr == 3)
            aisPacket->fragNr = atoi(token);
        if(tokNr == 4)
            aisPacket->seqId = atoi(token);
        if(tokNr == 5)
            aisPacket->chanCode = *token;
        if(tokNr == 6)
            memcpy(aisPacket->payload, token, sizeof(aisPacket->payload));
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

//return unsigned float from binary string, 1 decimal place
float returnU1FloatFromBin(char *bin){
    int sz = strlen(bin);
    int cnt = 0;
    int cntRev = sz - 1;
    float floatU1 = 0.0;
    for(cnt = 0; cnt < sz; cnt++){
        if(bin[cnt] == '1'){
            floatU1 += power(2, cntRev);
        }
        cntRev--;
    }
    floatU1 *= 0.1;
    return floatU1;
}

float COGtmp_returnU1FloatFromBin(char *bin){
    int sz = strlen(bin);
    int cnt = 0;
    int cntRev = sz - 1;
    float floatU1 = 0.0;
    for(cnt = 0; cnt < sz; cnt++){
        if(bin[cnt] == '1'){
            floatU1 += power(2, cntRev);
        }
        cntRev--;
    }
    floatU1 *= 0.01;
    return floatU1;
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
        subStr[end - start + 1] = '\0';
        return 0;
    }else{
        printf("retSubstring error: end is greater than start.\n");
        return 1;
    }
}

void returnBinaryPayload(char *payl, aisP *aisPacket){
    int i = 0;

    size_t paylSz = strlen(payl);
    
    size_t testCnt = 0;

    char *concatstr = (char *) malloc(paylSz  * 6 * sizeof(char) + 1);
    concatstr[0] = '\0';
    
    while(payl[i] != '\0'){
        //To recover (de-armor) the six bits, subtract 48 from the ASCII character value; if the result is greater than 40 subtract 8
        int res1 = (payl[i] - 48);
        if(res1 > 40)
            res1 -= 8;

        //Return 6 bit ascii value
        char *sixbits = malloc(6 * sizeof(char));
        ret6bit(res1, sixbits);

        //add to conactenated string
        strncat(concatstr, sixbits, 6 * sizeof(char));
        testCnt+=6;
        
        free(sixbits);
        i++;
    }
    concatstr[paylSz  * 6 * sizeof(char)] = '\0';
    memcpy(aisPacket->binaryPayload, concatstr, paylSz  * 6 * sizeof(char));
    free(concatstr); //this seems to be conflicting with out of scope de-alloc?
}

//return ascii value of six bit nibble strings
void returnAsciiFrom6bits(char *binString, aisP *aisPacket){
    size_t sz = strlen(binString);
    char *vesselName = malloc(((sz / 6) + 1) * sizeof(char));

    //Sanity check, vessel names not allways reliable
    if(sz % 6 != 0){
        printf("Binary string is not a multiple of 6, so probably invalid\n");
        memcpy(vesselName, "_", sz);
    }else{

        size_t i = 0;
        size_t j = 0;
        size_t k = 0;
        char *nibble = malloc(6 * sizeof(char));

        for(i = 0; i < sz; i++){
            nibble[j] = binString[i];
            if(j == 5){
                vesselName[k] = sixbitAscii[returnUIntFromBin(nibble)].ascii;
                j = 0;
                k++;
            }else{
                j++;
            }
        }
        vesselName[sz / 6] = '\0';
        memcpy(aisPacket->vesselName, vesselName, sizeof(aisPacket->vesselName));
    }
    free(vesselName);
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}
