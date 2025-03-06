
#define _CRT_SECURE_NO_WARNINGS
#define PROFILER 1

//h
#include "types.h"
#include "profiler_includes_single.h"

//cpp
#include "ishak_buffer.cpp"
#include "json_parser_single.cpp"
#include <cmath>

function_global f64 Square(f64 A)
{
    f64 Result = (A*A);
    return Result;
}

function_global f64 RadiansFromDegrees(f64 Degrees)
{
	f64 Result = 0.01745329251994329577 * Degrees;	
	return Result;	
}

// NOTE(casey): EarthRadius is generally expected to be 6372.8
function_global f64 ReferenceHaversine(f64 X0, f64 Y0, f64 X1, f64 Y1, f64 EarthRadius)
{
    /* NOTE(casey): This is not meant to be a "good" way to calculate the Haversine distance.
       Instead, it attempts to follow, as closely as possible, the formula used in the real-world
       question on which these homework exercises are loosely based.
    */
    
	f64 lat1 = Y0;
    f64 lat2 = Y1;
    f64 lon1 = X0;
    f64 lon2 = X1;
    
    f64 dLat = RadiansFromDegrees(lat2 - lat1);
    f64 dLon = RadiansFromDegrees(lon2 - lon1);
    lat1 = RadiansFromDegrees(lat1);
    lat2 = RadiansFromDegrees(lat2);
    
    f64 a = Square(sin(dLat/2.0)) + cos(lat1)*cos(lat2)*Square(sin(dLon/2));
    f64 c = 2.0*asin(sqrt(a));
    
    f64 Result = EarthRadius * c;
    
    return Result;
}

struct haversine_pair
{
    f64 X0, X1;
    f64 Y0, Y1;
};

function_global f64 SumHaversineDistances(u64 PairCount, haversine_pair *Pairs)
{
    printf("Pairs Num %llu \n", PairCount);
    PROFILE_BLOCK_BANDWITH(__func__, PairCount * sizeof(haversine_pair));
    f64 EarthRadius = 6372.8f;
    f64 Sum = 0;
    f64 SumCoef = 1 / (f64)PairCount;
    for(u64 PairIndex = 0; PairIndex < PairCount; ++PairIndex)
    {
        haversine_pair Pair = Pairs[PairIndex];
        f64 Dist = ReferenceHaversine(Pair.X0, Pair.Y0, Pair.X1, Pair.Y1, EarthRadius);
        Sum += SumCoef * Dist;
    }
    
    printf("Entire Sum of Haversine %.4f \n", Sum);
    
    
    return Sum;
}

int main(int ArgsCount,char** Args)
{   
    profiler::BeginProfiling();
    
    json_object json("input.json");
    
    haversine_pair* HaversineArray = nullptr;        
    u64 PairCount = 0;
    
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
                    ++PairCount;
                }
            }
        }    
        
    }
    
    
    f64 TotalSum = SumHaversineDistances(PairCount, HaversineArray);   
    
    profiler::EndProfiling();         
    free(HaversineArray);
    
	return 0;
}

PROFILING_ASSERT_CHECK();
