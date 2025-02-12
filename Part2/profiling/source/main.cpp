
// cpp
#include "cpu_defines.h"

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
    PROFILE_FUNCTION();
    int aa = GetCPUFrequency(1000);
    return  1;
}

#if PROFILER_PROJECT // Defined in the CMakeLists as a preprocessor definition

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

#endif // PROFILER_PROJECT
