#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

typedef struct sixbitAsciiTable{
    char bits[7];
    char ascii;
}sbat;

sbat sixbitAscii[64] = {
    {"000000", '@'},{"000001", 'A'},{"000010", 'B'},
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

//return ascii value of six bit nibbles
void returnAsciiFrom6bits(char *binString){
    size_t sz = strlen(binString);
    if(sz % 6 != 0){
        printf("Binary string is not a multiple of 6, so probably invalid\n");
    }

    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
    char *nibble = malloc(6 * sizeof(char));
    char *vesselName = (char *) malloc(sizeof(char) * sz + 1);
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
    printf("\nVesselname: %s\n", vesselName);
    free(vesselName);
}

int main(void){
    char mystr[] = "001000000101000001010100001000000101010010001001001110000001000000000000000000000000000000000000000000000000000000000000"; //HEATHERINA
    returnAsciiFrom6bits(mystr);
    return 0;
}
