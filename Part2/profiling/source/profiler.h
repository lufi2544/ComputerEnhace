/* date = January 20th 2025 11:25 am */

#ifndef PROFILER_H
#define PROFILER_H

#include "types.h"
#include "cpu_defines.h"
#include "string.h"

#define PROFILE_POINTS_NUM 4096


//Maybe an array of max Profile Functions that we can have, then when finished profiling, we dump the data to the console
// Static assert about the max profilers we can have.


#define NameConcat2(A, B) A##B
#define NameConcat(A, B) NameConcat2(A, B)

namespace profiler {         
    
    struct profiler_anchor
    {
        u64 TSCElapsed;
        u64 TSCPUElapsed;
        u64 HitCount;
        char const* Label;
    };
    
    struct core
    {
        profiler_anchor Anchors[4096];        
        
        u64 StartTS;
        u64 EndTS;
        
        u64 StartTSCPU;
        u64 EndTSCPU;
    };
            
    extern core GlobalProfiler;           
    struct profile_block
    {
        profile_block() = default;       
        profile_block(char const *Label_, u32 AnchorIndex_)
        {
            AnchorIndex = AnchorIndex_;
            Label = Label_;
            StartTS = ReadOSTimer();
            StartCPUTS = ReadCPUTimer();
        }             
                      
        ~profile_block()
        {                    
            u64 Elapsed = ReadOSTimer() - StartTS;
            u64 CPUElapsed = ReadCPUTimer() - StartCPUTS;
            
            profiler_anchor *Anchor = GlobalProfiler.Anchors + AnchorIndex;
            Anchor->TSCElapsed += Elapsed;
            Anchor->TSCPUElapsed += CPUElapsed;
            
            // TODO Investigate.
            Anchor->Label = Label;
        }
        
        char const* Label;
        u64 StartTS;
        u64 StartCPUTS;
        u32 AnchorIndex;        
    };      
    
    inline void BeginProfiling()
    {
        GlobalProfiler.StartTS = ReadOSTimer();        
        GlobalProfiler.StartTSCPU = ReadCPUTimer();
    }
    
    inline void PrintTimeElapsed(u64 TotalElapsed, u64 OSTSCFrequency, profiler_anchor *Anchor)
    {
        u64 Elapsed = Anchor->TSCElapsed;
        
        f64 ElapsedMiliseconds = ((f64)Elapsed / (f64)OSTSCFrequency) * 1000;
        
        u64 CPUElapsed = Anchor->TSCPUElapsed;        
        f64 Percent = 100.0 * ((f64)Elapsed / (f64)TotalElapsed);
        
        printf("   %s[%llu]: %.4fms (%.2f%%) CPU: %llu\n", Anchor->Label, Anchor->HitCount, ElapsedMiliseconds, Percent, CPUElapsed);
        
    }
    
    inline void EndProfiling()
    {
        
        GlobalProfiler.EndTS = ReadOSTimer();
        GlobalProfiler.EndTSCPU = ReadCPUTimer();
        u64 TotalTimeElapsed = GlobalProfiler.EndTS - GlobalProfiler.StartTS;
        u64 OSTimeStampCounterTimerFrequency = GetOSTimerFrequency();
        
        // Info Stamp to the Global Profilers
        
        for(u32 AnchorIndex = 0; AnchorIndex < ArrayCount(GlobalProfiler.Anchors); ++AnchorIndex)
        {
            profiler_anchor* Anchor = GlobalProfiler.Anchors + AnchorIndex;
            if(Anchor->TSCElapsed)
            {
                PrintTimeElapsed(TotalTimeElapsed, OSTimeStampCounterTimerFrequency, Anchor);
            }
        }
        
        f64 GlobalTimePassedMS = ((f64)TotalTimeElapsed / OSTimeStampCounterTimerFrequency) * 1000;
        u64 GlobalPassedCPU = GlobalProfiler.EndTSCPU - GlobalProfiler.StartTSCPU;
        printf("   Global Profiling Scope: %.4f ms CPU: %llu \n", GlobalTimePassedMS, GlobalPassedCPU);
            
    }
       
    
} //::profiler

#define PROFILE_BLOCK(Name) profiler::profile_block NameConcat(Block, __LINE__)(Name, __COUNTER__ + 1/* Reserving the 0 Index here  */);
#define PROFILE_FUNCTION() PROFILE_BLOCK(__func__)

#define PROFILING_ASSERT_CHECK() static_assert(__COUNTER__  <= (ArrayCount(profiler::core::Anchors)), "Number of ProfilePoints Exceeded...");
#define PROFILE_SCOPE() \


#endif //PROFILER_H

