/*
tcp_pthread_coban

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
#include <time.h>
#include <stdarg.h> //varargs
#define LOGLVL 0
#define MAXLOGLVL 4
// Thanks a lot to these helpful posts: 
// https://cboard.cprogramming.com/c-programming/172128-question-regarding-global-variable-c-new-post.html
 
struct logVar{
    FILE *lfp;
};
static const char *sevs[4] = {
    "debug","info","warn","crit"
};

static struct logVar logVars; 

void initLogr(const char *LOGFILE){
    logVars.lfp=fopen(LOGFILE, "a");
    if(logVars.lfp == NULL){
        printf("Error opening log file '%s', exit..\n",LOGFILE);
        exit(EXIT_FAILURE);
    }
    setlinebuf(logVars.lfp);
}

void termLogr(){
    fclose(logVars.lfp);
}

void logr(const int sev, const char *msg, ...){
    if(sev >= LOGLVL && sev < MAXLOGLVL){

        char buf[256];
        va_list args;
        va_start (args, msg);
        vsnprintf (buf, 256, msg, args);
        va_end(args);

        time_t t = time(NULL);
        char strftBuf[50];
        strftime(strftBuf, sizeof strftBuf, "%c", localtime(&t));

        fprintf(logVars.lfp, "%s :: %s\t%s\n", strftBuf, sevs[sev], buf);
    }
}
