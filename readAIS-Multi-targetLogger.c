#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "readAIS-Multi-targetLogger.h"
#include "readAIS-Multi-parse.h"
#include "gpsTools.h"
#define clear() printf("\033[H\033[J") //to clear the linux term

//Silly function, needs to be replaced
unsigned int ret1st3Dgts(unsigned int MMSI){
    size_t cnt =0;
    while(MMSI != 0){
       if(cnt == 6)
           return MMSI;
       MMSI /= 10;
       cnt++;
    }
    return 0;
}

void returnCntyCodes(struct cntyCodes *cc); 

//Returns country name, use locally
void returnCntyName(char *currCnty, unsigned int cntyCode, struct cntyCodes *cc){
    size_t cnt = 0;
    while(cc[cnt].code != 0){
        if(cc[cnt].code == cntyCode){
            memcpy(currCnty, cc[cnt].abbrev, sizeof(cc[cnt].abbrev));
            currCnty[2] = '\0';
            break;
        }
        cnt++;
    }
}

void updateTarget(atl *targetLog, aisP * aisPacket, gpsPos *myPos){
    time_t currentTime = time(NULL);
    atl *pushList = targetLog;

    while(pushList->next != NULL){
        if(pushList->MMSI == aisPacket->MMSI){
            pushList->heading = aisPacket->heading;
            pushList->cog = aisPacket->cog;
            pushList->sog = aisPacket->sog;
            pushList->lat = aisPacket->lat;
            pushList->lon = aisPacket->lon;
            pushList->lastUpdate = currentTime;
            pushList->dst = calcDistance(myPos->lat, myPos->lon, aisPacket->lat, aisPacket->lon);
            break;
        }
        pushList = pushList->next;
    }
}

void pushTarget(struct aisTargetLog *targetLog, aisP *aisPacket, struct cntyCodes *cc, gpsPos *myPos){
    time_t currentTime = time(NULL);
    atl *pushList = targetLog;

    while(pushList->next != NULL){
        pushList = pushList->next;
    }

    //Country code stuff
    char currCnty[3];
    returnCntyName(currCnty, ret1st3Dgts(aisPacket->MMSI), cc);

    pushList->next = malloc(sizeof(struct aisTargetLog));
    memcpy(pushList->vesselName, aisPacket->vesselName, sizeof(aisPacket->vesselName));
    memcpy(pushList->cnty, currCnty, sizeof(currCnty));;
    pushList->msgType = aisPacket->msgType;
    pushList->MMSI = aisPacket->MMSI;
    pushList->heading = aisPacket->heading;
    pushList->cog = aisPacket->cog;
    pushList->sog = aisPacket->sog;
    pushList->lat = aisPacket->lat;
    pushList->lon = aisPacket->lon;
    pushList->lastUpdate = currentTime;
    pushList->dst = calcDistance(myPos->lat, myPos->lon, aisPacket->lat, aisPacket->lon);
    pushList->length = 0;
    pushList->next->next = NULL;
}

void printTargetList(struct aisTargetLog *targetLog){
    clear();
    atl *alist = targetLog; //Pointer to targetLog
    time_t currentTime = time(NULL);
    char staleNote[8] = "\0";
    int maxAge = 8; //Target age in minutes
    size_t cnt = 0;
    size_t cntC = 0;
    
    printf("Nr\tType\tMMSI\t\tSog\tCog\tLat/ Lon\t\tDst\tCnty\tLen (m)\tVesselName\n");
    while(alist->next != NULL){
        //denote "stale" targets
        if(alist->lastUpdate < (currentTime - (60 * maxAge -2 )))
            memcpy(staleNote, "(stale)\0", 8);
        else
            staleNote[0] = '\0';

        if(alist->lastUpdate > (currentTime - (60 * (maxAge)))){
            printf("%d:\t(%d)\t%i\t%.2f\t%.2f°\t%.6f %.6f\t%.2f\t%s\t%d\t%s %s\n", cnt,\
                alist->msgType, alist->MMSI,
                alist->sog, alist->cog, 
                alist->lat, alist->lon, alist->dst, \
                alist->cnty, alist->length,
                alist->vesselName, staleNote);
            cntC++;
        }
        cnt++;

        alist = alist->next;
    }
    printf("In summary: %d targets in list, %d active.\n", cnt, cntC);
}

_Bool isNewTarget(atl *targetLog, aisP * aisPacket){
    atl *alist = targetLog;
    while(alist->next != NULL){
        if(alist->MMSI == aisPacket->MMSI){
            return 0;
        }
        alist = alist->next;
    }
    return 1;
}

void updateVesselDetails(atl *targetLog, aisP * aisPacket){
    time_t currentTime = time(NULL);
    atl *alist = targetLog;
    while(alist->next != NULL){
        if(alist->MMSI == aisPacket->MMSI){
            alist->lastUpdate = currentTime;
            alist->length = aisPacket->length;
            printf("assigned length from packet to alist: %d :: %d for mmsi %d\n", aisPacket->length, alist->length, alist->MMSI); 
            break;
        }
        alist = alist->next;
    }
}
void updateVesselName(atl *targetLog, aisP * aisPacket){
    time_t currentTime = time(NULL);
    atl *alist = targetLog;
    while(alist->next != NULL){
        if(alist->MMSI == aisPacket->MMSI){
            memcpy(alist->vesselName, aisPacket->vesselName, sizeof(aisPacket->vesselName));
            alist->lastUpdate = currentTime;
            break;
        }
        alist = alist->next;
    }
}

//Manges AIS target list
void manageTargetList(aisP *aisPacket, struct aisTargetLog *targetLog, struct cntyCodes *cc){
    gpsPos myPos; //Get station current GPS coords
    returnGPSPos(&myPos);
            
    if(isNewTarget(targetLog, aisPacket)){
        if(aisPacket->msgType != 24)
            pushTarget(targetLog, aisPacket, cc, &myPos);
    }else{
        if(aisPacket->msgType != 24)
            updateTarget(targetLog, aisPacket, &myPos);
    }

    //Msg type 24, partno 0 contains vessel name
    if(aisPacket->msgType == 24 && aisPacket->partNo == 0)
        updateVesselName(targetLog, aisPacket);
    if(aisPacket->msgType == 24 && aisPacket->partNo == 1)
        updateVesselDetails(targetLog, aisPacket);

    if(aisPacket->msgType == 5 || aisPacket->msgType == 19){
        updateVesselName(targetLog, aisPacket);
    }

    printTargetList(targetLog);
}

void returnCntyCodes(struct cntyCodes *cc){
    struct cntyCodes cctmp[] = {
        {201, "Albania", "AL"},
        {202, "Andorra", "AD"},
        {203, "Austria", "AT"},
        {204, "Portugal", "PT"},
        {205, "Belgium", "BE"},
        {206, "Belarus", "BY"},
        {207, "Bulgaria", "BG"},
        {208, "Vatican", "VA"},
        {209, "Cyprus", "CY"},
        {210, "Cyprus", "CY"},
        {211, "Germany", "DE"},
        {212, "Cyprus", "CY"},
        {213, "Georgia", "GE"},
        {214, "Moldova", "MD"},
        {215, "Malta", "MT"},
        {216, "Armenia", "ZZ"},
        {218, "Germany", "DE"},
        {219, "Denmark", "DK"},
        {220, "Denmark", "DK"},
        {224, "Spain", "ES"},
        {225, "Spain", "ES"},
        {226, "France", "FR"},
        {227, "France", "FR"},
        {228, "France", "FR"},
        {229, "Malta", "MT"},
        {230, "Finland", "FI"},
        {231, "Faroe, Is", "FO"},
        {232, "United, Kingdom", "GB"},
        {233, "United, Kingdom", "GB"},
        {234, "United, Kingdom", "GB"},
        {235, "United, Kingdom", "GB"},
        {236, "Gibraltar", "GI"},
        {237, "Greece", "GR"},
        {238, "Croatia", "HR"},
        {239, "Greece", "GR"},
        {240, "Greece", "GR"},
        {241, "Greece", "GR"},
        {242, "Morocco", "MA"},
        {243, "Hungary", "HU"},
        {244, "Netherlands", "NL"},
        {245, "Netherlands", "NL"},
        {246, "Netherlands", "NL"},
        {247, "Italy", "IT"},
        {248, "Malta", "MT"},
        {249, "Malta", "MT"},
        {250, "Ireland", "IE"},
        {251, "Iceland", "IS"},
        {252, "Liechtenstein", "LI"},
        {253, "Luxembourg", "LU"},
        {254, "Monaco", "MC"},
        {255, "Portugal", "PT"},
        {256, "Malta", "MT"},
        {257, "Norway", "NO"},
        {258, "Norway", "NO"},
        {259, "Norway", "NO"},
        {261, "Poland", "PL"},
        {262, "Montenegro", "ME"},
        {263, "Portugal", "PT"},
        {264, "Romania", "RO"},
        {265, "Sweden", "SE"},
        {266, "Sweden", "SE"},
        {267, "Slovakia", "SK"},
        {268, "San, Marino", "SM"},
        {269, "Switzerland", "CH"},
        {270, "Czech, Republic", "CZ"},
        {271, "Turkey", "TR"},
        {272, "Ukraine", "UA"},
        {273, "Russia", "RU"},
        {274, "FYR, Macedonia", "MK"},
        {275, "Latvia", "LV"},
        {276, "Estonia", "EE"},
        {277, "Lithuania", "LT"},
        {278, "Slovenia", "SI"},
        {279, "Serbia", "RS"},
        {301, "Anguilla", "AI"},
        {303, "USA", "US"},
        {304, "Antigua, Barbuda", "AG"},
        {305, "Antigua, Barbuda", "AG"},
        {306, "Curacao", "CW"},
        {307, "Aruba", "AW"},
        {308, "Bahamas", "BS"},
        {309, "Bahamas", "BS"},
        {310, "Bermuda", "BM"},
        {311, "Bahamas", "BS"},
        {312, "Belize", "BZ"},
        {314, "Barbados", "BB"},
        {316, "Canada", "CA"},
        {319, "Cayman, Is", "KY"},
        {321, "Costa, Rica", "CR"},
        {323, "Cuba", "CU"},
        {325, "Dominica", "DM"},
        {327, "Dominican, Rep", "DO"},
        {329, "Guadeloupe", "GP"},
        {330, "Grenada", "GD"},
        {331, "Greenland", "GL"},
        {332, "Guatemala", "GT"},
        {334, "Honduras", "HN"},
        {336, "Haiti", "HT"},
        {338, "USA", "US"},
        {339, "Jamaica", "JM"},
        {341, "St, Kitts, Nevis", "KN"},
        {343, "St, Lucia", "LC"},
        {345, "Mexico", "MX"},
        {347, "Martinique", "MQ"},
        {348, "Montserrat", "MS"},
        {350, "Nicaragua", "NI"},
        {351, "Panama", "PA"},
        {352, "Panama", "PA"},
        {353, "Panama", "PA"},
        {354, "Panama", "PA"},
        {355, "Panama", "PA"},
        {356, "Panama", "PA"},
        {357, "Panama", "PA"},
        {358, "Puerto, Rico", "PR"},
        {359, "El, Salvador", "SV"},
        {361, "St, Pierre, Miquelon", "PM"},
        {362, "Trinidad, Tobago", "TT"},
        {364, "Turks, Caicos, Is", "TC"},
        {366, "USA", "US"},
        {367, "USA", "US"},
        {368, "USA", "US"},
        {369, "USA", "US"},
        {370, "Panama", "PA"},
        {371, "Panama", "PA"},
        {372, "Panama", "PA"},
        {373, "Panama", "PA"},
        {374, "Panama", "PA"},
        {375, "St, Vincent, Grenadines", "VC"},
        {376, "St, Vincent, Grenadines", "VC"},
        {377, "St, Vincent, Grenadines", "VC"},
        {378, "British, Virgin, Is", "VG"},
        {379, "US, Virgin, Is", "VI"},
        {401, "Afghanistan", "AF"},
        {403, "Saudi, Arabia", "SA"},
        {405, "Bangladesh", "BD"},
        {408, "Bahrain", "BH"},
        {410, "Bhutan", "BT"},
        {412, "China", "CN"},
        {413, "China", "CN"},
        {414, "China", "CN"},
        {416, "Taiwan", "TW"},
        {417, "Sri, Lanka", "LK"},
        {419, "India", "IN"},
        {422, "Iran", "IR"},
        {423, "Azerbaijan", "AZ"},
        {425, "Iraq", "IQ"},
        {428, "Israel", "IL"},
        {431, "Japan", "JP"},
        {432, "Japan", "JP"},
        {434, "Turkmenistan", "TM"},
        {436, "Kazakhstan", "KZ"},
        {437, "Uzbekistan", "UZ"},
        {438, "Jordan", "JO"},
        {440, "Korea", "KR"},
        {441, "Korea", "KR"},
        {443, "Palestine", "PS"},
        {445, "DPR, Korea", "KP"},
        {447, "Kuwait", "KW"},
        {450, "Lebanon", "LB"},
        {451, "Kyrgyz, Republic", "ZZ"},
        {453, "Macao", "ZZ"},
        {455, "Maldives", "MV"},
        {457, "Mongolia", "MN"},
        {459, "Nepal", "NP"},
        {461, "Oman", "OM"},
        {463, "Pakistan", "PK"},
        {466, "Qatar", "QA"},
        {468, "Syria", "SY"},
        {470, "UAE", "AE"},
        {472, "Tajikistan", "TJ"},
        {473, "Yemen", "YE"},
        {475, "Yemen", "YE"},
        {477, "Hong, Kong", "HK"},
        {478, "Bosnia, and, Herzegovina", "BA"},
        {501, "Antarctica", "AQ"},
        {503, "Australia", "AU"},
        {506, "Myanmar", "MM"},
        {508, "Brunei", "BN"},
        {510, "Micronesia", "FM"},
        {511, "Palau", "PW"},
        {512, "New, Zealand", "NZ"},
        {514, "Cambodia", "KH"},
        {515, "Cambodia", "KH"},
        {516, "Christmas, Is", "CX"},
        {518, "Cook, Is", "CK"},
        {520, "Fiji", "FJ"},
        {523, "Cocos, Is", "CC"},
        {525, "Indonesia", "ID"},
        {529, "Kiribati", "KI"},
        {531, "Laos", "LA"},
        {533, "Malaysia", "MY"},
        {536, "N, Mariana, Is", "MP"},
        {538, "Marshall, Is", "MH"},
        {540, "New, Caledonia", "NC"},
        {542, "Niue", "NU"},
        {544, "Nauru", "NR"},
        {546, "French, Polynesia", "TF"},
        {548, "Philippines", "PH"},
        {553, "Papua, New, Guinea", "PG"},
        {555, "Pitcairn, Is", "PN"},
        {557, "Solomon, Is", "SB"},
        {559, "American, Samoa", "AS"},
        {561, "Samoa", "WS"},
        {563, "Singapore", "SG"},
        {564, "Singapore", "SG"},
        {565, "Singapore", "SG"},
        {566, "Singapore", "SG"},
        {567, "Thailand", "TH"},
        {570, "Tonga", "TO"},
        {572, "Tuvalu", "TV"},
        {574, "Vietnam", "VN"},
        {576, "Vanuatu", "VU"},
        {577, "Vanuatu", "VU"},
        {578, "Wallis, Futuna, Is", "WF"},
        {601, "South, Africa", "ZA"},
        {603, "Angola", "AO"},
        {605, "Algeria", "DZ"},
        {607, "St, Paul, Amsterdam, Is", "XX"},
        {608, "Ascension, Is", "IO"},
        {609, "Burundi", "BI"},
        {610, "Benin", "BJ"},
        {611, "Botswana", "BW"},
        {612, "Cen, Afr, Rep", "CF"},
        {613, "Cameroon", "CM"},
        {615, "Congo", "CG"},
        {616, "Comoros", "KM"},
        {617, "Cape, Verde", "CV"},
        {618, "Antarctica", "AQ"},
        {619, "Ivory, Coast", "CI"},
        {620, "Comoros", "KM"},
        {621, "Djibouti", "DJ"},
        {622, "Egypt", "EG"},
        {624, "Ethiopia", "ET"},
        {625, "Eritrea", "ER"},
        {626, "Gabon", "GA"},
        {627, "Ghana", "GH"},
        {629, "Gambia", "GM"},
        {630, "Guinea-Bissau", "GW"},
        {631, "Equ., Guinea", "GQ"},
        {632, "Guinea", "GN"},
        {633, "Burkina, Faso", "BF"},
        {634, "Kenya", "KE"},
        {635, "Antarctica", "AQ"},
        {636, "Liberia", "LR"},
        {637, "Liberia", "LR"},
        {642, "Libya", "LY"},
        {644, "Lesotho", "LS"},
        {645, "Mauritius", "MU"},
        {647, "Madagascar", "MG"},
        {649, "Mali", "ML"},
        {650, "Mozambique", "MZ"},
        {654, "Mauritania", "MR"},
        {655, "Malawi", "MW"},
        {656, "Niger", "NE"},
        {657, "Nigeria", "NG"},
        {659, "Namibia", "NA"},
        {660, "Reunion", "RE"},
        {661, "Rwanda", "RW"},
        {662, "Sudan", "SD"},
        {663, "Senegal", "SN"},
        {664, "Seychelles", "SC"},
        {665, "St, Helena", "SH"},
        {666, "Somalia", "SO"},
        {667, "Sierra, Leone", "SL"},
        {668, "Sao, Tome, Principe", "ST"},
        {669, "Swaziland", "SZ"},
        {670, "Chad", "TD"},
        {671, "Togo", "TG"},
        {672, "Tunisia", "TN"},
        {674, "Tanzania", "TZ"},
        {675, "Uganda", "UG"},
        {676, "DR, Congo", "CD"},
        {677, "Tanzania", "TZ"},
        {678, "Zambia", "ZM"},
        {679, "Zimbabwe", "ZW"},
        {701, "Argentina", "AR"},
        {710, "Brazil", "BR"},
        {720, "Bolivia", "BO"},
        {725, "Chile", "CL"},
        {730, "Colombia", "CO"},
        {735, "Ecuador", "EC"},
        {740, "UK", "UK"},
        {745, "Guiana", "GF"},
        {750, "Guyana", "GY"},
        {755, "Paraguay", "PY"},
        {760, "Peru", "PE"},
        {765, "Suriname", "SR"},
        {770, "Uruguay", "UY"},
        {775, "Venezuela", "VE"},
        {0, "endlist", "00"}
    };
    memcpy(cc, cctmp, sizeof(cctmp));
}
