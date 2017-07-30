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

unsigned int nmeaChecksumVerified(char *sentence){
    if(strlen(sentence) > 20){ //Test sentence length
        int checkSum = 0;
        size_t i;

        //Do xor checksum
        for(i = 1; i < strlen(sentence) - 5; i++){
            checkSum ^= sentence[i];
        }

        //Grab sentence checksum
        char *subStr = malloc(2 * sizeof(char));
        size_t start = strlen(sentence) - 4;
        size_t end = strlen(sentence) - 1;

        assignSubstring(sentence, start, end, subStr);
 
        _Bool rc = 0; //Return code
        int checkSumSentence= (int)strtol(subStr, NULL, 16); //Convert string to int

        if(checkSum == checkSumSentence) //checksums match?
                rc = 1;

        free(subStr);

        return rc;
    }else{
        return 0; //Sentence too short
    }
}

void parseMsg(char *line, aisP *aisPacket){
    //Do nema 0183 checksum
    if(nmeaChecksumVerified(line)){
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
    size_t bitStringSz = paylSz  * 6 * sizeof(char) + 1;
    char *bitString = malloc(bitStringSz + 1);
    bitString[0] = '\0'; //this is needed due to the way strncat works, or actually strlen
    
    while(payl[i] != '\0'){
        //To (de-armor) the six bits, subtract 48 from the ASCII character value; 
        int res1 = (payl[i] - 48);
        if(res1 > 40) //if the result is greater than 40 subtract 
            res1 -= 8;

        char *sixbits = malloc(7 * sizeof(char)); //Return 6 bit ascii value
        ret6bit(res1, sixbits); //returns six bit nibble for char
        
        strncat(bitString, sixbits, 6 * sizeof(char)); //conactenated bitstring
        
        free(sixbits);
        i++;
    }
    
    size_t nullTermPos = bitStringSz - 1;
    bitString[nullTermPos] = '\0';
    memcpy(aisPacket->binaryPayload, bitString, bitStringSz);
    free(bitString); 
}

//return ascii value of six bit nibble strings
void retShipnameFrom6bitsString(char *binString, aisP *aisPacket){
    size_t sz = strlen(binString);
    char *vesselName = malloc(((sz / 6) + 1) * sizeof(char));

    if(sz % 6 != 0){ //Sanity check, vessel names not allways reliable
        printf("Binary string is not a multiple of 6, so probably invalid\n");
        memcpy(vesselName, "_", sz);
    }else{
        size_t i = 0;
        size_t j = 0;
        size_t k = 0;
        char *nibble = malloc(7 * sizeof(char));

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
        size_t start = 0; //get message type
        size_t end = 5;
        char *subStr;
        start = 0;
        end = 5;
        subStr = malloc((end - start) + 2 * sizeof(char));

        assignSubstring(aisPacket->binaryPayload, start, end, subStr);
        assignUIntFromBin(subStr, &aisPacket->msgType);
        free(subStr);
        
        start = 124;//get heading
        end = 132;
        subStr = malloc((end - start) + 2 * sizeof(char));

        assignSubstring(aisPacket->binaryPayload, start, end, subStr);
        assignUIntFromBin(subStr, &aisPacket->heading);
        free(subStr);
        
        start = 8;//MMSI at offset 8-37, and convert to decimal
        end = 37;
        subStr = malloc((end - start) + 2 * sizeof(char));

        assignSubstring(aisPacket->binaryPayload, start, end, subStr);
        assignUIntFromBin(subStr, &aisPacket->MMSI);
        free(subStr);

        //For common navigation block, msg type 18 and 19
        if(aisPacket->msgType == 18 || aisPacket->msgType == 19){
            start = 57; //get lon
            end = 84;
            char *subStrLon = malloc((end - start) + 3 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStrLon);
            start = 85;//get lat 
            end = 111;
            char *subStrLat = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStrLat);
            assignLatLon(subStrLon, subStrLat, aisPacket);//set lat/lon
            free(subStrLon);
            free(subStrLat);

            start = 46;//get speed over ground 
            end = 55;
            subStr = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            aisPacket->sog= returnU1FloatFromBin(subStr);
            free(subStr);
            
            start = 112;//get cog class b
            end = 123;
            subStr = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            aisPacket->cog = returnU1FloatFromBin(subStr);
            free(subStr);
            
            start = 133;//get timestamp
            end = 138;
            subStr = malloc((end - start) + 3 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            assignUIntFromBin(subStr, &aisPacket->ts);
            free(subStr);

        //Common for type 1, 2 and 3 (class A transponders)
        }else if(aisPacket->msgType == 1\
                || aisPacket->msgType == 2\
                || aisPacket->msgType == 3\
                ){
            
            start = 50;//get speed over ground 
            end = 59;
            subStr = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            aisPacket->sog= returnU1FloatFromBin(subStr);
            free(subStr);

            start = 116;//get cog class a
            end = 127;
            subStr = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            aisPacket->cog = returnU1FloatFromBin(subStr);
            free(subStr);

            start = 61; // Get lon
            end = 88;
            char *subStrLon = malloc((end - start) + 3 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStrLon);
            
            start = 89;//getlat 
            end = 115;
            char *subStrLat = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStrLat);

            assignLatLon(subStrLon, subStrLat, aisPacket);//set lat/lon
            free(subStrLon);
            free(subStrLat);
        }
            
        //addendum to protocol for ais B transponders
        if(aisPacket->msgType == 24){
            start = 40;//get shipname
            end = 159;
            subStr = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            retShipnameFrom6bitsString(subStr, aisPacket);
            free(subStr);

            start = 38;//Get part nr of message
            end = 39;
            subStr = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            assignUIntFromBin(subStr, &aisPacket->partNo);
            free(subStr);
            
            //dimensions in M: 
            //- DtoBow: 132-140
            //- DtoStern: 141-149
            start = 132;//Get dimensions to Bow
            end = 140;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            size_t DtoBow;
            assignUIntFromBin(subStr, &DtoBow);
            free(subStr);

            start = 141;//Get dimensions to Stern
            end = 149;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            size_t DtoStern;
            assignUIntFromBin(subStr, &DtoStern);
            free(subStr);
            
            aisPacket->length = (DtoBow + DtoStern);
        }

        if(aisPacket->msgType == 19){ //msg type 19, extended report Class B
            start = 143;//get ship name
            end = 262;
            subStr = malloc((end - start) + 2 * sizeof(char));
            
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            retShipnameFrom6bitsString(subStr, aisPacket);
            free(subStr);
        }

        if(aisPacket->msgType == 5){//Static voyage related data ais class A
            start = 112;//get ship name
            end = 231;
            subStr = malloc((end - start) + 2 * sizeof(char));

            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            retShipnameFrom6bitsString(subStr, aisPacket);
            free(subStr);

            //dimensions in M: 
            //- DtoBow: 240-248
            //- DtoStern: 249-257
            start = 240;//Get dimensions to Bow
            end = 248;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            size_t DtoBow;
            assignUIntFromBin(subStr, &DtoBow);
            free(subStr);

            start = 249;//Get dimensions to Stern
            end = 257;
            subStr = malloc((end - start) + 2 * sizeof(char));
            assignSubstring(aisPacket->binaryPayload, start, end, subStr);
            size_t DtoStern;
            assignUIntFromBin(subStr, &DtoStern);
            free(subStr);
            
            aisPacket->length = (DtoBow + DtoStern);
        }
}
