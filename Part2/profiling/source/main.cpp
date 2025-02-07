
#define METRICS2

// cpp
#include "cpu_defines.h"
#include "metrics_base.cpp"

// h
#include "profiler.h"


#include "Task1.cpp"

int AmazingFunction()
{
    PROFILE_FUNCTION();
    int a = 0;
    while(a < 100000)
    {
        a++;
    }
    
    return 1;
}

int Lolete()
{
    //PROFILE_FUNCTION();
    
    return 0;
}

int lolete()
{
    //PROFILE_FUNCTION();
    return 1;
}

int main(int ArgsNum, const char** Args)
{       
    int a =  AmazingFunction(); 
    
    return 0;
}

static_assert(__COUNTER__ <= ArrayCount(profiler::ProfilePoints), "Number of ProfilePoints Exceeded...");


