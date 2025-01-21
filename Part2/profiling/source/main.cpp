
#define METRICS2

// cpp
#include "cpu_defines.h"
#include "metrics_base.cpp"

// h
#include "profiler.h"


#include "Task1.cpp"

int main(int ArgsNum, const char** Args)
{
    int a = 0;
    
    u64 Result = 0;
    {       
        Profiler prof("Task1 Loop", &Result);
        while(a < 1000000000)
        {
            a++;
        }
    }
    
    printf("RESULT: %f \n", Result);
    
}
