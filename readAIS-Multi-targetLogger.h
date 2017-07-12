#ifndef readAISMultiparse_H_
#define readAISMultiparse_H_

struct aisTargetLog{
    char vesselName[40];
    size_t msgType;
    unsigned int MMSI;
    size_t heading;
    float cog;
    float sog;
    float lat;
    float lon;
    struct aisTargetLog *next;
};


#endif
