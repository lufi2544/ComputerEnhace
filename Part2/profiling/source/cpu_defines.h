#include <intrin.h>
#include <windows.h>

#include "types.h"


/** This is the CPU Clock Cycles since CPU Started. */
inline u64 ReadCPUTimer(void)
{
    return __rdtsc();
}

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


static u64 GetCPUFrequency(u64 MilisecondsToWait)
{
    u64 OSFreq = GetOSTimerFreq(); // OS Ticks per second
    
    u64 CPUStart = ReadCPUTimer(); // CPU cycles time stamp
    u64 OSStart= ReadOSTimer(); // OS Ticks time stamp
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    u64 OSWaitTime = OSFreq * MilisecondsToWait / 1000; // Get the Ticks to wait for the Passed in Miliseconds to Seconds
    
    // Increment the Time Elapsed until we reach desired Ticks
    while(OSElapsed < OSWaitTime)
    {
        OSEnd = ReadOSTimer();
        OSElapsed = OSEnd - OSStart;       
    }
    
    u64 CPUEnd = ReadCPUTimer(); // Read the CPU cylces time stamp here
    u64 CPUElapsed = CPUEnd - CPUStart; // See the Diff for the CPU cycles
    u64 CPUFreq = 0;
    if(OSElapsed)
    {
        // As we know the CPU elapsed cycles and the ticks elapsed, we can figure out
        // the cycles in a single OS Tick and then multiply by the Ticks per second and 
        // figure out the CPU Cycles per Second.
        CPUFreq = OSFreq * CPUElapsed / OSElapsed;
    }
    
    printf("  OSTimer: %llu - %llu = %llu elapsed \n", OSStart, OSEnd, OSElapsed);
    printf(" OS Seconds: %.4f \n", (f64)OSElapsed / (f64)OSFreq);
    
    printf("  CPU Timer: %llu -> %llu = %llu elapsed  \n", CPUStart, CPUEnd, CPUElapsed);
    printf("  CPU Freq: %llu (guessed) \n", CPUFreq);
    
    return CPUFreq;
}

#else // TODO MacOS

//..

#endif // _WIN32