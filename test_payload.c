#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations

struct sixbitAsciiTable{
    char bits[7];
    char ascii;
};

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

//return ascii value of six bit nibble strings
void returnAsciiFrom6bits(char *binString, char *vesselName2){
    size_t sz = strlen(binString);
    char *vesselName = malloc(((sz / 6) + 1) * sizeof(char));

    //Sanity check, vessel names not allways reliable
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
        memcpy(vesselName2, vesselName, ((sz / 6) + 1));
    free(vesselName);
}

int main(void){
    //get ship name and more from type 24 message, correlation through MMSI
    char *binPayload = malloc(900 * sizeof(char)); 
    //memcpy(binPayload, "011000000011100000001101100001001000110000100100111000111000100101001110000000011000000100110000110000010100111010000010000010000010000010000010000010000010000000000110000000110000000100010001001000000100110110000010000010000010000000011110000001101000000111000011000011000100000000011000111100000000000000000000000000000000000000000000000000000000000000000000", 361);
    memcpy(binPayload, "011000000011100000001101100001001000110100011110000001001101000011100000100000100000100000110010001000000011001101110110100000100000000000100000000110000010000010000000000000110000000100010001001000000100110110000010000010000010000000011110000001101000000111000011000011000100000000011000111100000000000000000000000000000000000000000000000000000000000000000000", 361);
    char *vesselName2 = malloc(900 * sizeof(char));
    size_t start = 0; // def 40
    size_t end = 119; //def 159

    size_t cnt;
    for(cnt = 0; cnt < end - start; cnt++){ ;
        char *subStr = malloc((end - start) + 2 * sizeof(char));
        assignSubstring(binPayload, start, end , subStr);
        returnAsciiFrom6bits(subStr, vesselName2);
	printf("%i: Vesselname: (offset: %d-%d) %s\n", cnt, start, end, vesselName2);
    	free(subStr);
	start++;
	end++;
    }

    return 0;
}
