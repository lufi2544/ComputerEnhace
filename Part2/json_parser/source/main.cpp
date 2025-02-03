#include "json_parser.h"
#include "profiler.h"
#define _CRT_SECURE_NO_WARNINGS

int main(int ArgsCount,char** Args)
{   /*
    f64 Profiler_Parsing = 0;
    f64 Profiler_Printing = 0;
    {
        Profiler a("Parsing Json", &Profiler_Parsing);
        json_object json("input.json");   
        {
            Profiler b("Printing Json", &Profiler_Printing);
            json.Print();
        }
    }
    
    printf("Parsing-Printing Proportion: %.8f \n", Profiler_Printing / Profiler_Parsing);
    */

    int a = GetCPUFrequency();
    int b = 0;


}
