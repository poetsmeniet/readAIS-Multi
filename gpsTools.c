#include <stdio.h>
#include <math.h>
#include "gpsTools.h"
//Dont forget to link with -lm

//code adapted from Salvador Dali, stackoverflow. Many thanks!
//Applies haversine method between decimal degrees lat/lon
//usage:
//printf("%f\n", calcDistance(42.738705, -9.038047, 42.762424, -8.948540));
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

void returnGPSPos(gpsPos *myPos){
    myPos->lat = 42.738705;
    myPos->lon = -9.0380474;
}
