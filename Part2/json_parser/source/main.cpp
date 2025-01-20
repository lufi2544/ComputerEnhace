#include "json_parser.h"
#include "profiler.h"
#define _CRT_SECURE_NO_WARNINGS

int main(int ArgsCount,char** Args)
{
    f64 Res = 0;
    {
        Profiler prof("Json Parser Global Scope", &Res);
        json_object json("input.json");
        json.Print();
    }
    
    return 0;
}
