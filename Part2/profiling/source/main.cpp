
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

int AmazingFunction_2()
{
    PROFILE_FUNCTION();
    int a = 0;
    while(a < 100000)
    {
        a++;
    }
    
    return 1;    
}

int Mock()
{
    return  1;
}

int main(int ArgsNum, const char** Args)
{       
    profiler::BeginProfiling();
    
    int a =  AmazingFunction();
    int b = AmazingFunction_2();    
    Mock();
    
    profiler::EndProfiling();
    
    return 0;
}

PROFILING_ASSERT_CHECK()