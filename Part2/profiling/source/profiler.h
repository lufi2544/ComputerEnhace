/* date = January 20th 2025 11:25 am */

#ifndef PROFILER_H
#define PROFILER_H

#include "types.h"
#include "cpu_defines.h"


struct Profiler
{
    Profiler(const char* ScopeNameParam, f64* ElapsedTimeParam = nullptr, bool bVerboseParam = true)
        : ScopeName { ScopeNameParam }
    , ElapsedTime { ElapsedTimeParam }
    , bVerbose { bVerboseParam }
    {
        StartOSTicksTimeStmap = ReadOSTimer();
        CPUCyclesStart = ReadCPUTimer();
    }
    
    ~Profiler()
    {

        // TODO creating special case for MACOS
        u64 EndOSTicksTimeStamp = ReadOSTimer();
        u64 CPUCyclesEnd = ReadCPUTimer();
        u64 OSTicksDiff = EndOSTicksTimeStamp - StartOSTicksTimeStmap;
        
        // Compute elapsed time in milliseconds with higher precision
        f64 ElapsedTimeMiliseconds = ((f64)OSTicksDiff / (f64)GetOSTimerFrequency()) * 1000;
        
        if(ElapsedTime)
        {
            *ElapsedTime  = ElapsedTimeMiliseconds;                    
        }
        
        if(bVerbose)
        {            
            printf("%s Scope: Elapsed: Time: %.8f ms, CPUCycles: %llu \n", ScopeName, ElapsedTimeMiliseconds, CPUCyclesEnd - CPUCyclesStart);   
        }
    }
    
    
    const char* ScopeName = nullptr;
    u64 StartOSTicksTimeStmap = 0;        
    u64 CPUCyclesStart = 0;
    f64* ElapsedTime = nullptr;
    bool bVerbose = true;
};
#endif //PROFILER_H
