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

//sbat sixbitAscii = malloc(65 * sizeof(sbat));
sbat sixbitAscii[65] = {
    {"000000", '@'},
    {"000001", 'A'},
    {"000010", 'B'},
    };

//return ascii value of six bit nibbles
void returnAsciiFrom6bits(char *binString){
    size_t sz = strlen(binString);
    printf("lngth of str: %d\n", strlen(binString));

    printf("6bit ascii char value: %c\n", sixbitAscii[returnUIntFromBin(binString)].ascii);

}

int main(void){
    char mystr[] = "000000"; //P
    returnAsciiFrom6bits(mystr);
}
