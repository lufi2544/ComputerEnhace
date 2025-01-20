
#include <stdint.h>
#include <stdio.h>
#include "types.h"

#ifdef METRICS1
int main(int ArgsNum, const char** Args)
{
    u64 OSFreq = GetOSTimerFreq();
    printf("     OS Freq : %llu\n", OSFreq);
    
    u64 OSStart = ReadOSTimer();
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    while(OSElapsed < OSFreq)
    {
        OSEnd = ReadOSTimer();
        OSElapsed = OSEnd - OSStart;
    }
    
    printf("    OS Timer: %llu -> %llu = %llu elapsed \n", OSStart, OSEnd, OSElapsed);
    printf("  OS Seconds: %.4f\n", (f64)OSElapsed/(f64)OSFreq);
}
#endif