
#ifdef __APPLE__
#include "string.h"
#endif

#define NameConcat2(A, B) A##B
#define NameConcat(A, B) NameConcat2(A, B)


//Maybe an array of max Profile Functions that we can have, then when finished profiling, we dump the data to the console
// Static assert about the max profilers we can have.
#define PROFILE_POINTS_NUM 4096

namespace profiler {         
    
#if PROFILER
    struct profiler_anchor
    {
        u64 TSCElapsedExclusive;
        u64 TSCElapsedInclusive;
        u64 TSCPUElapsed;
        u64 HitCount;
        u64 ProcessByteCount;
        char const* Label;
        bool bIsBlock;
    };    
    
    global u32 GlobalProfilerParent;
    global profiler_anchor GlobalProfilerAnchors[4096];                    
    
    // RECURSION
    /**
     * Function flag, when we reach that function, we recursively check up the ParentIndexes with X depth and then 
     * if we reach to a parent with the same Index
     * 
    */    
    struct profile_block
    {
        profile_block() = default;       
        profile_block(char const *Label_, u32 AnchorIndex_, bool bIsBlock_, u64 ByteCount)
        {
            ParentIndex = GlobalProfilerParent;
            AnchorIndex = AnchorIndex_;
            Label = Label_;
            
            profiler_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;
            Anchor->HitCount++;
            Anchor->bIsBlock = bIsBlock_;
            Anchor->ProcessByteCount += ByteCount;
            OldTSCElapsedInclusive = Anchor->TSCElapsedInclusive;
            
            // Time Stamp 
            StartTS = ReadOSTimer();
            StartCPUTS = ReadCPUTimer();            
            
            GlobalProfilerParent = AnchorIndex_;            
        }             
        
        ~profile_block()
        {                    
            u64 Elapsed = ReadOSTimer() - StartTS;
            u64 CPUElapsed = ReadCPUTimer() - StartCPUTS;
            GlobalProfilerParent = ParentIndex;
            
            profiler_anchor *Parent = GlobalProfilerAnchors + ParentIndex;
            profiler_anchor *Anchor = GlobalProfilerAnchors + AnchorIndex;
            
            // Excluding the time passed from the parent and then adding the time passed to this Anchor Exclusive time
            // since children may subtract it from it as well.
            Parent->TSCElapsedExclusive -= Elapsed;
            Anchor->TSCElapsedExclusive += Elapsed;
            Anchor->TSCPUElapsed += CPUElapsed;
            Anchor->TSCElapsedInclusive = OldTSCElapsedInclusive + Elapsed;
            
            // TODO Investigate.
            Anchor->Label = Label;            
            
        }
        
        char const* Label;
        u64 StartTS;
        u64 StartCPUTS;
        u64 OldTSCElapsedInclusive;
        u32 AnchorIndex;        
        u32 ParentIndex;       
    }; 
    
    function_global void PrintTimeElapsed(u64 TotalElapsed, u64 OSTSCFrequency, profiler_anchor *Anchor)
    {
        f64 ElapsedMiliseconds = ((f64)Anchor->TSCElapsedExclusive / (f64)OSTSCFrequency) * 1000;        
        u64 CPUElapsed = Anchor->TSCPUElapsed;
        f64 Percent = 100.0 * ((f64)Anchor->TSCElapsedExclusive / (f64)TotalElapsed);
        
        if(Anchor->bIsBlock)
        {            
            printf("   Block %s[%llu]: %.4fms (%.2f%%) CPU: %llu ", Anchor->Label, Anchor->HitCount, ElapsedMiliseconds, Percent, CPUElapsed);            
        }
        else
        {
            printf("   %s[%llu]: %.4fms (%.2f%%) CPU: %llu ", Anchor->Label, Anchor->HitCount, ElapsedMiliseconds, Percent, CPUElapsed);            
        }
        
        if(Anchor->TSCElapsedInclusive != Anchor->TSCElapsedExclusive)
        {
            f64 PercentWithChildren = 100.0 * ((f64)Anchor->TSCElapsedInclusive / (f64)TotalElapsed);
            printf(" (%.2f%%) w/children ) ", PercentWithChildren);           
        }         
        
        if(Anchor->ProcessByteCount)
        {
            f64 Megabyte = 1024 * 1024;
            f64 Gigabyte = Megabyte * 1024;
            
            f64 Seconds = (f64)Anchor->TSCElapsedInclusive / (f64)OSTSCFrequency;
            f64 BytesPerSecond = (f64)Anchor->ProcessByteCount / (f64)Seconds;
            f64 Megabytes = (f64)Anchor->ProcessByteCount / (f64)Megabyte;
            f64 GygabytesPerSecond = BytesPerSecond/ Gigabyte;
            
            
            printf(" %.3fmb at %.2f gb/s", Megabytes, GygabytesPerSecond);
            
            
        }
        
        printf("\n");
    }
    
    
    function_global void PrintAnchorsTime(u64 TotalTimeElapsed, u64 OSTimeStampCounterTimerFrequency)
    {
        for(u32 AnchorIndex = 1; AnchorIndex < ArrayCount(GlobalProfilerAnchors); ++AnchorIndex)
        {
            profiler_anchor* Anchor = GlobalProfilerAnchors + AnchorIndex;
            if(Anchor->TSCElapsedExclusive)
            {
                PrintTimeElapsed(TotalTimeElapsed, OSTimeStampCounterTimerFrequency, Anchor);
            }
        }
    }
    
#define PROFILE_BLOCK_BANDWITH(Name, Value,  ByteCount) profiler::profile_block NameConcat(Block, __LINE__)(Name, __COUNTER__ + 1, Value, ByteCount);
    
#define PROFILE_BLOCK(Name) PROFILE_BLOCK_BANDWITH(Name, true, 0)
#define PROFILE_FUNCTION()  PROFILE_BLOCK_BANDWITH(__func__, false, 0)
#define PROFILING_ASSERT_CHECK() static_assert(__COUNTER__  <= (ArrayCount(profiler::GlobalProfilerAnchors)), "Number of ProfilePoints Exceeded...");
    
    
#else
    
#define PrintElapsedTimer(...)
#define PrintAnchorsTime(...) 
    
#define PROFILE_BLOCK_BANDWITH(...) 
#define PROFILE_BLOCK(...)
#define PROFILE_FUNCTION(...)
#define PROFILING_ASSERT_CHECK()
    
#endif // PROFILER
    
    struct core
    {        
        u64 StartTS;
        u64 EndTS;
        
        u64 StartTSCPU;
        u64 EndTSCPU;
    };
    
    global core GlobalProfiler;   
    
    inline void BeginProfiling()
    {
        GlobalProfiler.StartTS = ReadOSTimer();        
        GlobalProfiler.StartTSCPU = ReadCPUTimer();
    }
    
    
    function_global void EndProfiling()
    {
        
        GlobalProfiler.EndTS = ReadOSTimer();
        GlobalProfiler.EndTSCPU = ReadCPUTimer();
        u64 TotalTimeElapsed = GlobalProfiler.EndTS - GlobalProfiler.StartTS;
        u64 OSTimeStampCounterTimerFrequency = GetOSTimerFrequency();
        
        // Info Stamp to the Global Profilers
        printf("-------------------------- PROFILER -------------------------------------- \n");
        printf("\n");                
        
        PrintAnchorsTime(TotalTimeElapsed, OSTimeStampCounterTimerFrequency);        
        
        f64 GlobalTimePassedMS = ((f64)TotalTimeElapsed / OSTimeStampCounterTimerFrequency) * 1000;
        u64 GlobalPassedCPU = GlobalProfiler.EndTSCPU - GlobalProfiler.StartTSCPU;
        printf("   Global Profiling Scope: %.4f ms CPU: %llu \n", GlobalTimePassedMS, GlobalPassedCPU);
        
        printf("\n");
        printf("-------------------------------------------------------------------------- \n");
    }
    
    
    
} //::profiler
