
#pragma once
#include "types.h"

#ifdef _WIN32
#include <intrin.h>
#include <windows.h>

/** This is the CPU Clock Cycles since CPU Started.
 * 
 * The processor monotonically increments the time-stamp counter MSR every clock cycle and resets it to 0 whenever the processor is reset.
 * rdtsc - Read Time Stamp Counter
 */
inline u64 ReadCPUTimer(void)
{
    return __rdtsc();
}

/*
 *Retrieves the frequency of the performance counter. The frequency of the performance counter is fixed at system boot and is consistent
 * across all processors. Therefore, the frequency need only be queried upon application initialization, and the result can be cached.

 @juanes: This are the OS Ticks in 1 second.
*/
static u64 GetOSTimerFrequency(void)
{
    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    
    return Freq.QuadPart;
}

/*
 *Retrieves the current value of the performance counter, which is a high resolution (<1us) time stamp that can be used for time-interval measurements.
*
* @juanes: This is the OS Ticks since system started. This gets the Time Stamp Counter from the CPU and conversts that to 10MHz which is 
* QueryPerformanceFrequency.
*/
static u64 ReadOSTimer(void)
{
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    
    return Value.QuadPart;
}




#elif defined (__APPLE__)

#include "mach/mach_time.h"

// Read the ARM Performance Monitors Cycle Counter
static inline u64 ReadCPUTimer(void)
{
    
    u64 cntvct;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(cntvct));
    return cntvct;
}

static u64 GetOSTimerFrequency()
{
    mach_timebase_info_data_t TimeBase;
    mach_timebase_info(&TimeBase);
    return (u64)(1e9 * (double)TimeBase.denom / (double)TimeBase.numer);
}


static u64 ReadOSTimer()
{
    return mach_absolute_time();
}

#else // TODO implement Linux

#error Platform not supported

#endif // _WIN32

static u64 GetCPUFrequency(u64 MilisecondsToWait = 100)
{
    u64 OSFreq = GetOSTimerFrequency(); // OS Ticks per second
    
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
        //
        // This is a Rule of Third basically.
        CPUFreq = OSFreq * (CPUElapsed / OSElapsed);
    }
    
    printf("  OSTimer: %llu - %llu = %llu elapsed \n", OSStart, OSEnd, OSElapsed);
    printf(" OS Seconds: %.4f \n", (f64)OSElapsed / (f64)OSFreq);
    
    printf("  CPU Timer: %llu -> %llu = %llu elapsed  \n", CPUStart, CPUEnd, CPUElapsed);
    printf("  CPU Freq: %llu (guessed) \n", CPUFreq);
    
    return CPUFreq;
}
