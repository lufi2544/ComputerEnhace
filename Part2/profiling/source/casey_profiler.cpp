#include "types.h"

struct profiler_anchor
{
    u64 TSCElapsed;
    u64 HitCount;
    char const* Label;
};


struct profiler
{
    profile_anchor Anchors[4096];
    
    u64 StartTSC;
    u64 EndTSC;
};

static profiler GlobalProfiler;

struct profile_block
{
    profile_block(char const *Label_, u32 AnchorIndex_)
    {
        AnchorIndex = AnchorIndex_;
        Label = Label_;
        StartTSC = ReadCPUTimer();
    }
    
    ~profile_block()
    {
        u64 Elapsed = ReadCPUTimer() - StartTSC;
        
        profile_anchor *Anchor = GlobalProfiler.Anchors + AnchorIndex;
        Anchor->TSCElapsed += Elapsed;
        ++Anchor->HitCount;
        
        Anchor->Label = Label;
    }
};