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
    char bits[6];
    char ascii;
}sbat;

sbat sixbitAscii[65] = {
    {"000000", '@'},{"000001", 'A'},{"000010", 'B'},
    {"000011", 'C'},{"000100", 'D'},{"000101", 'E'},
    {"000110", 'F'},{"000111", 'G'},{"001000", 'H'},
    {"001000", 'H'},{"001001", 'I'},{"001010", 'J'},
    {"001011", 'K'},{"001100", 'L'},{"001101", 'M'},
    {"001110", 'N'},{"001111", 'O'},{"100000", 'P'},
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
    char nibble[6];
    char *vesselname = (char *) malloc(sizeof(char) * (sz / 6) + 1);
    for(i = 0; i < sz; i++){
        nibble[j] = binString[i];
        if(j == 5){
            printf("6bit ascii char value: %c\n", sixbitAscii[returnUIntFromBin(nibble)].ascii);
            vesselname[k] = sixbitAscii[returnUIntFromBin(nibble)].ascii;
            j = 0;
            k++;
        }else{
            j++;
        }
    }
    printf("Vesselname: %s\n", vesselname);
    free(vesselname);

}

int main(void){
    char mystr[] = "000111000101"; //P
    returnAsciiFrom6bits(mystr);
    return 0;
}
