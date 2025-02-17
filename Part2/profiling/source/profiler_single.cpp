
#ifdef __APPLE__
#include "string.h"
#endif

#define NameConcat2(A, B) A##B
#define NameConcat(A, B) NameConcat2(A, B)


//Maybe an array of max Profile Functions that we can have, then when finished profiling, we dump the data to the console
// Static assert about the max profilers we can have.
#define PROFILE_POINTS_NUM 4096

namespace profiler {         
    
    struct profiler_anchor
    {
        u64 TSCElapsed;
        u64 TSCPUElapsed;
        u64 TSCElapsedChildren;
        u64 HitCount;
        char const* Label;
        
        // TODO Flags for the Recursiveness
        bool bIsFunction;
        bool bIsRecursive;        
        u32 ParentIndex;
        u32 Index;
    };
    
    struct core
    {
        profiler_anchor Anchors[4096];        
        
        u64 StartTS;
        u64 EndTS;
        
        u64 StartTSCPU;
        u64 EndTSCPU;
    };
    
    global core GlobalProfiler;   
    global u32 GlobalProfilerParent;
    
    #define RECURSION_CHECK_DEPTH 10 /* Depth of looking up for parent index with same index as current function scope. */
    
    // RECURSION
    /**
     * Function flag, when we reach that function, we recursively check up the ParentIndexes with X depth and then 
     * if we reach to a parent with the same Index
     * 

    */
    
    struct profile_block
    {
        profile_block() = default;       
        profile_block(char const *Label_, u32 AnchorIndex_, bool bIsFunction_)
        {
            bIsFunction = bIsFunction_;
            ParentIndex = GlobalProfilerParent;
            AnchorIndex = AnchorIndex_;
            Label = Label_;
            StartTS = ReadOSTimer();
            StartCPUTS = ReadCPUTimer();            
            
            GlobalProfilerParent = AnchorIndex_;
            
            profiler_anchor *Anchor = GlobalProfiler.Anchors + AnchorIndex;
            profiler_anchor *Parent = GlobalProfiler.Anchors + ParentIndex;
            Anchor->bIsFunction = bIsFunction;
            Anchor->bIsRecursive = Parent->bIsRecursive;
            Anchor->ParentIndex = ParentIndex;
            Anchor->Index = AnchorIndex;
            Anchor->HitCount++;
                       
            if(!Anchor->bIsRecursive)
            {                
                // Recursive Check
                u32 RecursionCheck = 0;
                u32 IndexToCheck = ParentIndex;
                while(RecursionCheck < RECURSION_CHECK_DEPTH)
                {
                    
                    profiler_anchor *AnchorToCheck = GlobalProfiler.Anchors + IndexToCheck;            
                    if(AnchorToCheck->bIsFunction && AnchorToCheck->Index == AnchorIndex && AnchorToCheck->TSCElapsed == 0/* No Time Stamped yet */)
                    {
                        // Recursion
                        AnchorToCheck->bIsRecursive = true;
                        break;
                    }
                    
                    IndexToCheck = AnchorToCheck->ParentIndex;
                    RecursionCheck++;
                }
            }

        }             
        
        ~profile_block()
        {                    
            u64 Elapsed = ReadOSTimer() - StartTS;
            u64 CPUElapsed = ReadCPUTimer() - StartCPUTS;
            GlobalProfilerParent = ParentIndex;
            
            profiler_anchor *Parent = GlobalProfiler.Anchors + ParentIndex;
            profiler_anchor *Anchor = GlobalProfiler.Anchors + AnchorIndex;
            
            if(!Parent->bIsRecursive && Anchor->bIsFunction && Anchor->bIsRecursive)
            {
                printf("---------------------------- \n");
                printf("Detected Recursion --> Label: %s Reached %llu times.\n", Label, Anchor->HitCount);
                printf("---------------------------- \n");
            }
            
            Parent->TSCElapsedChildren += Elapsed;
            Anchor->TSCElapsed += Elapsed;
            Anchor->TSCPUElapsed += CPUElapsed;
            
            // TODO Investigate.
            Anchor->Label = Label;
        }
        
        char const* Label;
        u64 StartTS;
        u64 StartCPUTS;
        u32 AnchorIndex;        
        u32 ParentIndex;
        bool bIsFunction;
    };      
    
    inline void BeginProfiling()
    {
        GlobalProfiler.StartTS = ReadOSTimer();        
        GlobalProfiler.StartTSCPU = ReadCPUTimer();
    }
    
    function_global void PrintTimeElapsed(u64 TotalElapsed, u64 OSTSCFrequency, profiler_anchor *Anchor)
    {
        u64 Elapsed = Anchor->TSCElapsed - Anchor->TSCElapsedChildren;        
        f64 ElapsedMiliseconds = ((f64)Elapsed / (f64)OSTSCFrequency) * 1000;
        
        u64 CPUElapsed = Anchor->TSCPUElapsed;        
        f64 Percent = 100.0 * ((f64)Elapsed / (f64)TotalElapsed);
        
        printf("   %s[%llu]: %.4fms (%.2f%%) CPU: %llu ", Anchor->Label, Anchor->HitCount, ElapsedMiliseconds, Percent, CPUElapsed);
        if(Anchor->TSCElapsedChildren)
        {
            f64 PercentWithChildren = 100.0 * ((f64)Anchor->TSCElapsed / (f64)TotalElapsed);
            printf(" (%.2f%%) w/children ", PercentWithChildren);
        }            
        
        printf("\n");
    }
    
    function_global void EndProfiling()
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

#define PROFILE_BLOCK_(Name, Val) profiler::profile_block NameConcat(Block, __LINE__)(Name, __COUNTER__ + 1/* Reserving the 0 Index here  */, Val);
#define PROFILE_BLOCK(Name) PROFILE_BLOCK_(Name, false)
#define PROFILE_FUNCTION()  PROFILE_BLOCK_(__func__, true)

#define PROFILING_ASSERT_CHECK() static_assert(__COUNTER__  <= (ArrayCount(profiler::core::Anchors)), "Number of ProfilePoints Exceeded...");

