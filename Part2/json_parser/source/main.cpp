
#define _CRT_SECURE_NO_WARNINGS
#define PROFILER 1

//h
#include "types.h"
#include "profiler_includes_single.h"

//cpp
#include "ishak_buffer.cpp"
#include "json_parser_single.cpp"



struct haversine_pair
{
    f64 X0, X1;
    f64 Y0, Y1;
};

int main(int ArgsCount,char** Args)
{   
    profiler::BeginProfiling();
    
    json_object json("input.json");
    json.Print();
    
    haversine_pair* HaversineArray = nullptr;        
    {
        PROFILE_BLOCK("Getting Values");
        bool bFound;
        json_value PairsValue = json.GetValue(CONSTANT_STRING("pairs"), bFound);
        if(bFound)
        {
            json_array PairsArray = PairsValue.JsonArray;
            if(PairsArray.Size > 0)
            {
                // Allocating memory for the haversine pairs.
                HaversineArray = (haversine_pair*)malloc(sizeof(haversine_pair) * PairsArray.Size);
                
                json_array_value ArrayValue = PairsArray.Value;
                for(u32 ArrayIndex = 0; ArrayIndex < PairsArray.Size; ++ArrayIndex)
                {
                    json_object* PairJson = ArrayValue.JsonObjectArray[ArrayIndex];
                    if(PairJson->Size == 0)
                    {
                        printf("JSON is empty and it shouldn't. %s \n", PairJson->Name);
                        continue;
                    }
                    
                    json_value x0_Value = PairJson->GetValue(CONSTANT_STRING("x0"), bFound);
                    json_value x1_Value = PairJson->GetValue(CONSTANT_STRING("y0"), bFound);                
                    json_value y0_Value = PairJson->GetValue(CONSTANT_STRING("x1"), bFound);
                    json_value y1_Value = PairJson->GetValue(CONSTANT_STRING("y1"), bFound);
                    
                    haversine_pair toAdd  = haversine_pair{ x0_Value.Number, x1_Value.Number, y0_Value.Number, y1_Value.Number };
                    HaversineArray[ArrayIndex] = toAdd; 
                }
            }
        }    
    }
    
    profiler::EndProfiling();         
    free(HaversineArray);
    
    return 0;
}

PROFILING_ASSERT_CHECK();
