
#define METRICS2

// cpp
#include "cpu_defines.cpp"
#include "metrics_base.cpp"

// h
#include "profiler.h"


#include "Task1.cpp"

int main(int ArgsNum, const char** Args)
{
    int a = 0;
    
    {
        Profiler prof("Task1 Loop");
        while(a < 100000000)
        {
            a++;
        }
    }
    
}
