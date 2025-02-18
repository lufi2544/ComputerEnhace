
// h
#include "types.h"

// cpp
#include "cpu_defines_single.cpp"
#include "profiler_single.cpp"

int recursive_function(int num)
{
    
    PROFILE_FUNCTION();
    if(num)
    {
        num--;
        num+= recursive_function(num);        
    }
    
    return num;
}

void AmazingFunction()
{
    PROFILE_FUNCTION();
    int a = 0;
    while(a < 1000000)
    {
        a++;
    }        
}

int AmazingFunction_2()
{
    PROFILE_FUNCTION();
    int a = 0;
    while(a < 100000)
    {
        PROFILE_BLOCK("Loop");
        a++;
    }
    
    return 1;    
}

 
int Mock()
{
    PROFILE_FUNCTION();
    GetCPUFrequency(1000);
    return  1;
}

#ifdef PROFILER_PROJECT // Defined in the CMakeLists as a preprocessor definition

int main(int ArgsNum, const char** Args)
{       
    profiler::BeginProfiling();   
    
    int a = recursive_function(100);   
    
    profiler::EndProfiling();
    
    
    printf("Result: %i \n", a);
    
    return 0;
}

PROFILING_ASSERT_CHECK()

#endif // PROFILER_PROJECT
