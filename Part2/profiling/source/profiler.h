/* date = January 20th 2025 11:25 am */

#ifndef PROFILER_H
#define PROFILER_H

#include "types.h"
#include "cpu_defines.h"
#include "string.h"

#define PROFILE_POINTS_NUM 4096


//Maybe an array of max Profile Functions that we can have, then when finished profiling, we dump the data to the console
// Static assert about the max profilers we can have.

namespace profiler {         
    
    struct profiler_anchor
    {
        u64 TSCElapsed;
        u64 HitCount;
        char const* Label;
    };
    
    struct core
    {
        profile_anchor Anchors[4096];
        
        u64 StartTSC;
        u64 EndTSC;
    };
    
    extern profiler GlobalProfiler;
    
    
    extern u16 ProfilePointsNum;
    extern struct ProfilerHandler ProfilePoints[PROFILE_POINTS_NUM];        
    
    struct ProfilerHandler
    {
        ProfilerHandler() = default;
        ProfilerHandler(const char* ScopeNameParam, bool bIsGlobalProfilerParam = false)
            : bIsGlobalProfiler { bIsGlobalProfilerParam }
        
        {                        
            s64 ScopeNameLen = strlen(ScopeNameParam) + 1;
            ScopeName = (char*)malloc(sizeof(char) * ScopeNameLen);
            memset(ScopeName, '\0', ScopeNameLen);
            memcpy(ScopeName, ScopeNameParam, ScopeNameLen);
            
            ID = ProfilePointsNum++;
            
            ProfileStart();
        }
        
        void ProfileStart()
        {
            // Compute elapsed time in milliseconds with higher precision
            StartOSTicksTimeStmap = ReadOSTimer();
            CPUCyclesStart = ReadCPUTimer();
        }
        
        void ProfileEnd()
        {
            u64 EndOSTicksTimeStamp = ReadOSTimer();
            u64 CPUCyclesEnd = ReadCPUTimer();
            u64 OSTicksDiff = EndOSTicksTimeStamp - StartOSTicksTimeStmap;
            
            //(juanes.rayo): in MACOS it seems that in user mode is impossible to access the CPU cycles, just in kernel mode... 
            // we expect it to be wrong data.
            // Compute elapsed time in milliseconds with higher precision
            f64 ElapsedTimeMiliseconds = ((f64)OSTicksDiff / (f64)GetOSTimerFrequency()) * 1000;
            ProfiledMiliseconds = ElapsedTimeMiliseconds;
            
            /*if(bVerbose)
            {            
                printf("%s Function: Elapsed: Time: %.8f ms, CPUCycles: %llu \n", ScopeName, ElapsedTimeMiliseconds, CPUCyclesEnd - CPUCyclesStart); 
            }*/
            
            //Info Stamp the Profiled info.
            
            ProfilePoints[ID] = *this;
        }
        
        ~ProfilerHandler()
        {
            if(!bIsGlobalProfiler)
            {
                ProfileEnd();
            }
        }
        
        
        char* ScopeName = nullptr;
        u64 StartOSTicksTimeStmap = 0;        
        u64 CPUCyclesStart = 0;
        f64 ProfiledMiliseconds = 0;
        u16 ID = 0;
        bool bIsGlobalProfiler = false;
    };
    
    
    extern ProfilerHandler global_profiler;
    
    inline void BeginProfiling()
    {
        global_profiler = ProfilerHandler("global_profiler", true);
        __COUNTER__;
    }
    
    inline void EndProfiling()
    {
        global_profiler.ProfileEnd();
        const f64 global_time = global_profiler.ProfiledMiliseconds;
        
        // Info Stamp to the Global Profilers
        
        for(u16 ProfilerIndex = 0; ProfilerIndex < ProfilePointsNum; ProfilerIndex++)
        {
            ProfilerHandler profiler = ProfilePoints[ProfilerIndex];
            if(profiler.bIsGlobalProfiler)
            {
                continue;
            }
            
            // print global info
            printf("::%s %.4f ms. Total Percentage: %.4f \n", profiler.ScopeName, profiler.ProfiledMiliseconds, (profiler.ProfiledMiliseconds / global_time) * 100);
            free(profiler.ScopeName);
        }
        
        printf("%s scope: %.4f ms. \n", global_profiler.ScopeName, global_time);
        free(global_profiler.ScopeName);
    }
    
} //::profiler

#define PROFILE_FUNCTION() \
profiler::ProfilerHandler function_profiler(__FUNCTION__); \
__COUNTER__; \

#define PROFILING_ASSERT_CHECK() static_assert(__COUNTER__  <= (ArrayCount(profiler::ProfilePoints)), "Number of ProfilePoints Exceeded...");
#define PROFILE_SCOPE() \


#endif //PROFILER_H

