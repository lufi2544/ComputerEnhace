
//h
#include "types.h"
#include "profiler_includes_single.h"

//cpp
#include "json_parser_single.cpp"

#define _CRT_SECURE_NO_WARNINGS

void Print(json_object* json)
{
    PROFILE_FUNCTION();
    json->Print();
}
int main(int ArgsCount,char** Args)
{   
    profiler::BeginProfiling();
    json_object json("input.json");       
    
    Print(&json);
    
    profiler::EndProfiling();        
    
    return 0;
}

PROFILING_ASSERT_CHECK();
