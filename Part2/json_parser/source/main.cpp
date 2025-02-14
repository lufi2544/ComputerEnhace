
#define _CRT_SECURE_NO_WARNINGS

//h
#include "types.h"
#include "profiler_includes_single.h"

//cpp
#include "json_parser_single.cpp"



int main(int ArgsCount,char** Args)
{   
    profiler::BeginProfiling();
    json_object json("input.json");       
    json.Print();        
    profiler::EndProfiling();        
    
    return 0;
}

PROFILING_ASSERT_CHECK();
