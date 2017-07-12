#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readAIS-Multi-targetLogger.h"
#include "readAIS-Multi-parse.h"

//Protocol description: http://catb.org/gpsd/AIVDM.html#_open_source_implementations
void manageTargetList(aisP *aisPacket, struct aisTargetLog *targetLog){
    // Check if target is already in list
    //
    // if in list
    //  update data
    // else
    //  insert data
}
