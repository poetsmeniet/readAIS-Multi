#include <stdio.h>

void ret6bit(char myChar, char *sixbits){
    //char myChar = 'A';
    int i;
    size_t bit = 0;
    int cnt = 5;
    for (i = 0; i < 7; i++) {
        //bit = !!((myChar << i) & 1);
        bit = myChar >> i & 1;
        sixbits[cnt] = bit + '0';
        cnt--;
    }
        sixbits[6] = '\0';
}

int main(void){
    char payl[] = "13P7fBIP00Olim<Lt3Vpkwv62L0<";
    //char payl[] = "B3P7E?000?u<HrW?09@Nwwp5oP06";
    int i = 0;

printf("Payload '%s' has %i chars\n", payl, (sizeof(payl) - 1));

    while(payl[i] != '\0'){
        //To recover (de-armor) the six bits, subtract 48 from the ASCII character value; if the result is greater than 40 subtract 8
        int res1 = (payl[i] - 48);
        if(res1 > 40)
            res1 -= 8;

        char sixbits[6];
        char testChar = 't';
        ret6bit(res1, sixbits);
        printf("'%c' = %i - 48 = %i \t :: bits: %s\n", payl[i], payl[i], res1, sixbits);

        //Concatenate all six-bit quantities found in the payload, MSB first. This is the binary payload of the sentence.
        //todo..
        
        i++;
    }
    //Data in AIS message payloads (what you get after undoing the AIVDM/AIVDO armoring) is encoded as bitfields in the sentence
    //
    //- Class A type 1,2,3 position report implementation:
    // (page 9)



}
