#include <intrin.h>
#include <windows.h>

#include "types.h"

#if _WIN32
static u64 GetOSTmerFreq(void)
{
    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    
    return Freq.QuadPart;
}

static u64 ReadOSTimer(void)
{
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    
    return Value.QuadPart;
}

#endif // _WIN32


inline u64 ReadCPUTimer(void)
{
    return __rdtsc();
}