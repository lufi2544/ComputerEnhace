
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
        PROFILE_BLOCK("Block");
        num--;
        num+= recursive_function(num);        
    }
    
    return num;
}

#ifdef PROFILER_PROJECT // Defined in the CMakeLists as a preprocessor definition

int main(int ArgsNum, const char** Args)
{       
    profiler::BeginProfiling();   
    
    int a = recursive_function(1000);   
    
    profiler::EndProfiling();
    
    
    printf("Result: %i \n", a);
    
    return 0;
}

PROFILING_ASSERT_CHECK()

#endif // PROFILER_PROJECT
