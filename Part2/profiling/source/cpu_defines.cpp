#include <intrin.h>
#include <windows.h>

#include "types.h"

#if _WIN32


/*
 *Retrieves the frequency of the performance counter. The frequency of the performance counter is fixed at system boot and is consistent  
 * across all processors. Therefore, the frequency need only be queried upon application initialization, and the result can be cached.

 @juanes: This are the OS Ticks in 1 second.
*/
static u64 GetOSTimerFreq(void)
{
    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    
    return Freq.QuadPart;
}

/*
 *Retrieves the current value of the performance counter, which is a high resolution (<1us) time stamp that can be used for time-interval measurements.

@juanes: This is the OS Ticks since system started.
*/
static u64 ReadOSTimer(void)
{
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    
    return Value.QuadPart;
}

#else // TODO MacOS

//..

#endif // _WIN32

/** This is the CPU Clock Cycles since CPU Started. */
inline u64 ReadCPUTimer(void)
{
    return __rdtsc();
}