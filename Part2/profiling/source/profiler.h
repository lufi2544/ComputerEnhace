/* date = January 20th 2025 11:25 am */

#ifndef PROFILER_H
#define PROFILER_H

#include "types.h"


struct Profiler
{
    Profiler(const char* ScopeNameParam)
        : ScopeName { ScopeNameParam }
    {
        StartOSTicksTimeStmap = ReadOSTimer();
        CPUCyclesStart = ReadCPUTimer();
    }
    
    ~Profiler()
    {
        u64 EndOSTicksTimeStamp = ReadOSTimer();
        u64 CPUCyclesEnd = ReadCPUTimer();
        u64 OSTicksDiff = EndOSTicksTimeStamp - StartOSTicksTimeStmap;
        
        printf("%s Scope: Elapsed: Time: %llu CPUCycles: %llu \n", ScopeName, OSTicksDiff, CPUCyclesEnd - CPUCyclesStart);
        
        f64 PassedSeconds = (f64)OSTicksDiff / (f64)GetOSTimerFreq();
        GetCPUFrequency(PassedSeconds * 1000);
    }
    
    
    const char* ScopeName = nullptr;
    u64 StartOSTicksTimeStmap = 0;        
    u64 CPUCyclesStart = 0;
};


#endif //PROFILER_H
