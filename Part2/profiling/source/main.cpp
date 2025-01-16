#include "cpu_metrics.cpp"

int main(void)
{
    u64 OSFreq = GetOSTmerFreq();
    printf("     OS Freq : %llu\n", OSFreq);
    
    u64 OSStart = ReadOSTimer();
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    while(OSElapsed < OSFreq)
    {
        OSEnd = ReadOSTimer();
        OSElapsed = OSEnd - OSStart;
    }
    
    printf("    OS Timer: %llu -> %llu = %llu elapsed \n", OSStart, OSEnd, OSElapsed);
    printf("  OS Seconds: %.4f\n", (f64)OSElapsed/(f64)OSFreq);
}