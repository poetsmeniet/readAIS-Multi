#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ret6bit(char myChar, char *sixbits){
    int i;
    size_t bit = 0;
    int cnt = 5;
    for (i = 0; i < 7; i++) {
        bit = myChar >> i & 1;
        sixbits[cnt] = bit + '0';
        cnt--;
    }
        sixbits[6] = '\0';
}

int main(void){
    char payl[] = "B3P=Ot000?u;tTW?G0L93w`UoP06";
    int i = 0;

    size_t paylSz = ((sizeof(payl) * sizeof(char)) - 1);
    printf("Payload '%s' has %i chars\n", payl, paylSz);

    char *concatstr = (char *) malloc(paylSz  * 6 * sizeof(char) + 1);

    while(payl[i] != '\0'){
        //To recover (de-armor) the six bits, subtract 48 from the ASCII character value; if the result is greater than 40 subtract 8
        int res1 = (payl[i] - 48);
        if(res1 > 40)
            res1 -= 8;

        char sixbits[6];
        ret6bit(res1, sixbits);
        //add to conactenated string
        strncat(concatstr, sixbits, (sizeof(sixbits)));

        printf("'%c' = %i - 48 = %i \t :: bits: %s\n", payl[i], payl[i], res1, sixbits);
        
        i++;
    }

    printf("\nConcat string: %s\n", concatstr);

    free(concatstr);
    return 0;
}
