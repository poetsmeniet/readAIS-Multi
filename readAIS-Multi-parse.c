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

unsigned int returnNmeaChecksum(char *sentence){
printf("start of checsum..\n");
    int checkSum = 0;
    size_t i;
    for(i = 1; i < strlen(sentence) - 3; i++){
        checkSum ^= sentence[i];
    }
printf("returned: %x\n", checkSum);
    return checkSum;
}

void parseMsg(char *line, aisP *aisPacket){
    //Extract fields of ais packet and store in struct for later decoding
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
void assignUIntFromBin(char *bin, unsigned int *target){
    int sz = strlen(bin);

    int cnt = 0;
    int cntRev = sz - 1;
    unsigned int decimal = 0;
    for(cnt = 0; cnt < sz; cnt++){
        if(bin[cnt] == '1'){
            decimal += power(2, cntRev);
        }
        cntRev--;
    }
    *target = decimal;
}

void assignIntFromBin(char *bin, int *target){
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
    *target = decimal;
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
    floatU1 *= 0.1;
    return floatU1;
}

void assignLatLon(char *lonBin, char *latBin, aisP *aisPacket){
    int lon;
    assignIntFromBin(lonBin, &lon);
    int valLon = lon & 0x08000000; 

    int lat;
    assignIntFromBin(latBin, &lat);
    int valLat = lat & 0x04000000;

    if(valLon)
        lon |= 0xf0000000;
    if(valLat)
        lat |= 0xf8000000;
        
    aisPacket->lon = lon / 600000.0;
    aisPacket->lat = lat / 600000.0;
}

//Returns an array of bits (6) of char
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
int assignSubstring(char *myStr, size_t start, size_t end, char *subStr){
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

//Returns the binary payload, ie information regarding sending station. De-armoring and supplies bitstring
void returnBinaryPayload(char *payl, aisP *aisPacket){
    int i = 0;

    size_t paylSz = strlen(payl);
    size_t testCnt = 0;
    size_t bitStringSz = paylSz  * 6 * sizeof(char) + 1;
    char *bitString = malloc(bitStringSz + 1);
    bitString[0] = '\0'; //still need to figure out the need for this 
    
    while(payl[i] != '\0'){
        //To (de-armor) the six bits, subtract 48 from the ASCII character value; 
        int res1 = (payl[i] - 48);
        if(res1 > 40) //if the result is greater than 40 subtract 
            res1 -= 8;

        char *sixbits = malloc(6 * sizeof(char)); //Return 6 bit ascii value
        ret6bit(res1, sixbits);
        
        strncat(bitString, sixbits, 6 * sizeof(char)); //conactenated string
        testCnt+=6;
        
        free(sixbits);
        i++;
    }
    
    size_t nullTermPos = bitStringSz - 1;
    bitString[nullTermPos] = '\0';
    memcpy(aisPacket->binaryPayload, bitString, bitStringSz);
    free(bitString); 
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
                nibble[6] = '\0';
                unsigned int tmp;
                assignUIntFromBin(nibble, &tmp);
                vesselName[k] = sixbitAscii[tmp].ascii;
                j = 0;
                k++;
            }else{
                j++;
            }
        }
        free(nibble);
        vesselName[sz / 6 - 1] = '\0';
        memcpy(aisPacket->vesselName, vesselName, sizeof(aisPacket->vesselName));
    }
    free(vesselName);
}

void printErr(char *msg){
    fprintf(stdout, "ERR: %s\n", msg);
}

void decodePayload(aisP * aisPacket){
        //get message type
        size_t start = 0;
        size_t end = 5;
        char *subStr;
        start = 0;
        end = 5;
        subStr = malloc((end - start) + 2 * sizeof(char));
        assignSubstring(aisPacket->binaryPayload, start, end, subStr);
        assignUIntFromBin(subStr, &aisPacket->msgType);
        free(subStr);
         
        //get heading
        start = 124;
        end = 132;
        subStr = malloc((end - start) + 2 * sizeof(char));
        assignSubstring(aisPacket->binaryPayload, start, end, subStr);
        assignUIntFromBin(subStr, &aisPacket->heading);
        free(subStr);
        
        //MMSI at offset 8-37, and convert to decimal
        start = 8;
        end = 37;
        subStr = malloc((end - start) + 2 * sizeof(char));
        assignSubstring(aisPacket->binaryPayload, start, end, subStr);
        assignUIntFromBin(subStr, &aisPacket->MMSI);
        free(subStr);

        //B
        //lon: 57-84
        //lat: 85-111
        if(aisPacket->msgType == 18\
                || aisPacket->msgType == 19){
            //get lon
            start = 57;
            end = 84;
            char *subStrLon = malloc((end - start) + 3 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStrLon);
            //get lat 
            start = 85;
            end = 111;
            char *subStrLat = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStrLat);
            //set lat/lon
            assignLatLon(subStrLon, subStrLat, aisPacket);
            free(subStrLon);
            free(subStrLat);

            //get speed over ground (std class b  CS position report
            start = 46;
            end = 55;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            aisPacket->sog= returnU1FloatFromBin(subStr);
            free(subStr);
            
            //get cog class b
            start = 112;
            end = 123;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            aisPacket->cog = COGtmp_returnU1FloatFromBin(subStr);
            free(subStr);
            
            //get timestamp
            start = 133;
            end = 138;
            subStr = malloc((end - start) + 3 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            assignUIntFromBin(subStr, &aisPacket->ts);
            free(subStr);
            
            
        }else if(aisPacket->msgType == 1\
                || aisPacket->msgType == 2\
                || aisPacket->msgType == 3\
                ){
            //get speed over ground class A
            start = 50;
            end = 59;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            aisPacket->sog= returnU1FloatFromBin(subStr);
            free(subStr);

            //get cog class a
            start = 116;
            end = 127;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            aisPacket->cog = COGtmp_returnU1FloatFromBin(subStr);
            free(subStr);

            //A
            //lon: 61-88
            //lat: 89-115
            //get lon
            start = 61;
            end = 88;
            char *subStrLon = malloc((end - start) + 3 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStrLon);
            //getlat 
            start = 89;
            end = 115;
            char *subStrLat = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStrLat);
            //set lat/lon
            assignLatLon(subStrLon, subStrLat, aisPacket);
            free(subStrLon);
            free(subStrLat);
        }
            
        //addendum to protocol for ais B transponders, integrate later to add to struct
        if(aisPacket->msgType == 24){
            //get ship name and more from type 24 message, correlation through MMSI
            start = 40;
            end = 159;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            returnAsciiFrom6bits(subStr, aisPacket);
            free(subStr);

            //Get part nr of message
            start = 38;
            end = 39;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            assignUIntFromBin(subStr, &aisPacket->partNo);
            free(subStr);
            //get cog class a, msgtype 24 (does not contain this info)
            aisPacket->cog = 0.0;
            
            //type 24, class B does not have lat/lon.. overwriting with 0 fr debug prints
            aisPacket->sog = 0;
            aisPacket->lon = 0;
            aisPacket->lat = 0;
        }

        //Static voyage related data ais class A
        if(aisPacket->msgType == 5){
            //get ship name and more from type 24 message, correlation through MMSI
            start = 112;
            end = 231;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            returnAsciiFrom6bits(subStr, aisPacket);
            free(subStr);
        }
}
