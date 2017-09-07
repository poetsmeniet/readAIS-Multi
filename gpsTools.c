/*
readAIS-Multi gps tools

Copyright (c) 2017 Thomas Wink <thomas@geenbs.nl>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gpsTools.h"
#include "readAIS-Multi-parse.h"
#define DEVICE "/dev/ttyUSB1"
//Dont forget to link with -lm

FILE *openGPSDevice();

//code adapted from Salvador Dali, stackoverflow. Many thanks!
//Applies haversine method between decimal degrees lat/lon
double calcDistance(double lat1, double lon1, double lat2, double lon2) {
    unsigned int R = 3440; // Radius of the earth in nm
    double pi = 3.141592653589793; 
    double dLat = (lat2 - lat1) * pi / 180;  
    double dLon = (lon2 - lon1) * pi / 180;
    double a = \
       0.5 - cos(dLat)/2 + \
       cos(lat1 * pi / 180) * cos(lat2 * pi / 180) * \
       (1 - cos(dLon))/2;
    
    return R * 2 * asin(sqrt(a));
}

//Converts degrees decimal minutes  token to decimal degrees
//Lat DDMM.MMMMM format, Lon DDDMM.MMMMM format
float degDecMin2DecDeg(char* degDecMin, char hem, size_t len){
        char deg[3],decmin[7]; 
        int sign = 1;

        if(len == 9){ //lat
            memcpy(deg, degDecMin+0, 2);
            memcpy(decmin, degDecMin+2, 7);
            if(hem == 'S')
                sign = -1.;
        }else if(len == 10){ //lon
            memcpy(deg, degDecMin+0, 3);
            memcpy(decmin, degDecMin+3, 8);
            if(hem == 'W')
                sign = -1.;
        }else{
            return 1; //Catch error
        }

        float decdeg=atoi(deg) + atof(decmin) / 60.;

        return decdeg * sign; 
}

//Extracts lat/lon from nmea 0183 sentence (gps dongle)
void returnGPSPos(gpsPos *myPos){
    char *line = malloc(sizeof(char) * 120);
    size_t len = 0;

    FILE *fp = openGPSDevice();
    char lat[10];
    char lon[11];

    while(1){
        getline(&line, &len, fp);

        if(!nmeaChecksumVerified(line))
                continue;

        char *token, *str, *tofree;
        tofree = str = strdup(line);  
        size_t tokNr = 1;
        size_t verif = 0;

        while ((token = strsep(&str, ","))){ 
            if(tokNr == 1) //Parse msg type $GPGGA
                verif = (strncmp(token, "$GPGGA", 6) == 0) ? 1 : 0;

            if(tokNr == 3 && verif == 1){
                memcpy(lat, token, 9);
                lat[9] = '\0';
            }
            if(tokNr == 4 && verif == 1)
                myPos->lat = degDecMin2DecDeg(lat, token[0], 9);
            
            if(tokNr == 5 && verif == 1){
                memcpy(lon, token, 10);
                lon[10] = '\0';
            }
            if(tokNr == 6 && verif == 1){
                myPos->lon = degDecMin2DecDeg(lon, token[0], 10);
                break;
            }
            tokNr++;
        }
        free(tofree);
        if(verif == 1)
            break;
    }
    fclose(fp);
    free(line);
}

FILE *openGPSDevice(){
    FILE *fp = fopen(DEVICE, "r");
    if(fp == NULL){
        printf("Unable to open device");
        exit(1);
    }
    return fp;
}
