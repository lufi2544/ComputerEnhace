#include "json_parser.h"
#include "profiler.h"
#define _CRT_SECURE_NO_WARNINGS


void PrintJson(json_object* Json)
{
    PROFILE_FUNCTION();
    Json->Print();
}

int main(int ArgsCount,char** Args)
{   
    profiler::BeginProfiling();
    json_object json("input.json");   
    {
        PrintJson(&json);
    }
    
    profiler::EndProfiling();        
    
    return 0;
}
