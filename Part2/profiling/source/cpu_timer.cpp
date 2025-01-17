#include <stdint.h>
#include <stdio.h>

#include "types.h"


#ifdef METRICS2
int main(void)
{
    u64 OSFreq = GetOSTimerFreq(); // OS Timer Ticks in 1 second, so if we wait until this ticks has passed, we can measure 1 raw second.
	printf("    OS Freq: %llu\n", OSFreq);
    
	u64 CPUStart = ReadCPUTimer(); // CPU clock cycles time stamp - cycles since the CPU started.
	u64 OSStart = ReadOSTimer(); // OS Ticks since system started. 
	u64 OSEnd = 0;
	u64 OSElapsed = 0;
	while(OSElapsed < OSFreq) // Meassured Ticks has reached the amount of Ticks in 1 second? if yes, then 1 second has passed.
	{
		OSEnd = ReadOSTimer();
		OSElapsed = OSEnd - OSStart;
	}
	
	u64 CPUEnd = ReadCPUTimer();
	u64 CPUElapsed = CPUEnd - CPUStart;
	
	printf("   OS Timer: %llu -> %llu = %llu elapsed\n", OSStart, OSEnd, OSElapsed);
	printf(" OS Seconds: %.4f\n", (f64)OSElapsed/(f64)OSFreq);
	
	printf("  CPU Timer: %llu -> %llu = %llu elapsed\n", CPUStart, CPUEnd, CPUElapsed); // And this is the CPU Clock Cycles in a second, which is the Frequency of the CPU. 3 GHz.
}

#endif