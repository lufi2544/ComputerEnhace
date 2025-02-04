
#pragma once
#include "types.h"

#ifdef _WIN32
#include <intrin.h>
#include <windows.h>

/** This is the CPU Clock Cycles since CPU Started. */
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

@juanes: This is the OS Ticks since system started.
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
/**
*	This is the CPU cycles since the CPU started in mac.
*/
static inline u64 ReadCPUTimer(void)
{
    
    u64 cntvct;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(cntvct));
    return cntvct;
}

/**
 * 	This is the frequency of the OS every second.
*/
static u64 GetOSTimerFrequency()
{

   /* __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(cntvct));*/

    mach_timebase_info_data_t TimeBase;
    mach_timebase_info(&TimeBase);

    // The mach_timebase_info gives you the conversion factor from time(ns) to ticks,
    // if we invert the conversion factor, we can get the ticks to time, and this would be the
    // Performance Timer Frequency time to ticks in nanoseconds, so we then convert it
    // to seconds.
    return 1e9 * ((double)TimeBase.denom / (double)TimeBase.numer);
}

/**
*	Time Stamp since the system started but in this case it should be a transfromed version
*	from the rdstc in mac.
*/
static u64 ReadOSTimer()
{
    /*mach_timebase_info_data_t TimeBase;
    mach_timebase_info(&TimeBase);

    // Convert the timebase into nanoseconds per tick
     auto a = (double)TimeBase.numer / (double)TimeBase.denom;

    int d = mach_absolute_time() * a;
    return d;*/

    return mach_absolute_time();
}

#else // TODO implement Linux

#error Platform not supported

#endif // _WIN32

    static u64 GetCPUFrequency(u64 MillisecondsToWait = 100)
    {
        u64 OSFreq = GetOSTimerFrequency();  // Get OS ticks per second
        u64 CPUStart = ReadCPUTimer();       // Initial CPU timestamp
        u64 OSStart = ReadOSTimer();         // Initial OS timestamp
        u64 OSEnd = 0;
        u64 OSElapsedTicks = 0;

        u64 OSWaiTicks = OSFreq * MillisecondsToWait / 1000;  // Convert to OS ticks

        while (OSElapsedTicks < OSWaiTicks)
        {
            OSEnd = ReadOSTimer();
            OSElapsedTicks = OSEnd - OSStart;
        }

        u64 CPUEnd = ReadCPUTimer();
        u64 CPUElapsed = CPUEnd - CPUStart;

        u64 CPUFreq = 0;
        if (OSElapsedTicks)
        {
            CPUFreq = OSFreq * (CPUElapsed / OSElapsedTicks);
        }

        printf("  OSTimer: %llu - %llu = %llu elapsed\n", OSStart, OSEnd, OSElapsedTicks);
        printf(" OS Seconds: %.4f\n", (double)OSElapsedTicks / (double)OSFreq);
        printf("  CPU Timer: %llu -> %llu = %llu elapsed\n", CPUStart, CPUEnd, CPUElapsed);
        printf("  CPU Freq: %llu (guessed)\n", CPUFreq);

        return CPUFreq;
    }
