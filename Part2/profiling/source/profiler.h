/* date = January 20th 2025 11:25 am */

#ifndef PROFILER_H
#define PROFILER_H

#include "types.h"
#include "cpu_defines.h"


#define PROFILE_FUNCTION() \
profiler::ProfilerHandler function_profiler(__FUNCTION__); \
__COUNTER__; \

#define PROFILE_SCOPE() \


//Maybe an array of max Profile Functions that we can have, then when finished profiling, we dump the data to the console
// Static assert about the max profilers we can have.

namespace profiler {               
    
    struct ProfilerHandler
    {
        ProfilerHandler() = default;
        ProfilerHandler(const char* ScopeNameParam, f64* ElapsedTimeParam = nullptr, bool bVerboseParam = true)
            : ScopeName { ScopeNameParam }
        , ElapsedTime { ElapsedTimeParam }
        , bVerbose { bVerboseParam }
        {
            StartOSTicksTimeStmap = ReadOSTimer();
            CPUCyclesStart = ReadCPUTimer();
        }
        
        ~ProfilerHandler()
        {
            
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
                printf("%s Function: Elapsed: Time: %.8f ms, CPUCycles: %llu \n", ScopeName, ElapsedTimeMiliseconds, CPUCyclesEnd - CPUCyclesStart); 
            }
        }
        
        
        const char* ScopeName = nullptr;
        u64 StartOSTicksTimeStmap = 0;        
        u64 CPUCyclesStart = 0;
        f64* ElapsedTime = nullptr;
        bool bVerbose = true;
    };
    
        
    extern ProfilerHandler ProfilePoints[1];        
}

#endif //PROFILER_H

