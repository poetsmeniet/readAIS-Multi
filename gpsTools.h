#ifndef GPSTOOLS_H_
#define GPSTOOLS_H_

typedef struct gpsPositionDD{
    double lat;
    double lon;
}gpsPos;

double calcDistance(double lat1, double lon1, double lat2, double lon2);
void returnGPSPos(gpsPos *myPos);
void returnGPSPosDev(gpsPos *myPos);
FILE *openGPSDevice();
#endif

