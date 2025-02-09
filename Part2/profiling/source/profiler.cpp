#include "profiler.h"

namespace profiler {
    
    ProfilerHandler ProfilePoints[PROFILE_POINTS_NUM];
    ProfilerHandler global_profiler = ProfilerHandler();
    u16 ProfilePointsNum = 0;
    
} // ::profiler