/* date = January 20th 2025 11:25 am */

#ifndef PROFILER_H
#define PROFILER_H

#include "types.h"
#include "cpu_defines.h"


struct Profiler
{
    Profiler(const char* ScopeNameParam, u64* ElapsedTimeParam)
        : ScopeName { ScopeNameParam }
        , ElapsedTime { ElapsedTimeParam }
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
        
        *ElapsedTime = ((f64)OSTicksDiff / (f64)GetOSTimerFrequency()) * 1000;
        GetCPUFrequency(*ElapsedTime);
    }
    
    
    const char* ScopeName = nullptr;
    u64 StartOSTicksTimeStmap = 0;        
    u64 CPUCyclesStart = 0;
    u64* ElapsedTime = nullptr;
};


#endif //PROFILER_H
