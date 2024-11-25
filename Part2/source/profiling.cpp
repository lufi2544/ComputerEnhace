
#include "types.h"
#include <iostream>
#include <random>
#include <cstdio>
#include "harvesine_formula.h"
#include "profiling.h"

#define _CRT_SECURE_NO_WARNINGS
#define U64Max  UINT64_MAX


struct random_series
{
	u64 A, B, C, D;
};

static u64
RotateLeft(u64 Value, int Times)
{
	u64 Result = (Value << Times) | (Value >> (64 - Times)); // Wrapping around the bits 10111111 << 3 | 10111111 >> (8 - 3) = 11111101.
	return Result;
}

static u64
RandomU64(random_series* Input)
{
	u64 A = Input->A;
	u64 B = Input->B;
	u64 C = Input->C;
	u64 D = Input->D;
	
	u64 E = A - RotateLeft(B, 27);
	A = (B ^ RotateLeft(C, 17));
	B = (C + D);
	C = (D + E);
	D = (E + A);
	
	Input->A = A;
	Input->B = B;
	Input->C = C;
	Input->D = D;
	
	return D;
}

static random_series
Seed(u64 Value)
{
	random_series Result;
	Result.A = 0xdeadbeef;
	Result.B = Value;
	Result.C = Value;
	Result.D = Value;
	
	u32 Count = 20;
	while(Count--)
	{
		RandomU64(&Result);
	}
	
	return Result;
}

static f64 RandomInRange(random_series* Series, f64 Min, f64 Max)
{
	int a  = 0;
	f64 t = (f64)RandomU64(Series) / (f64)U64Max;
	f64 Result = ((1 - t) * Min) + (t * Max);
	
	return Result;
}

static FILE*
Open(u64 NumOfPairs, const char* Name, const char* Extension)
{
	char Temp[256];
	sprintf(Temp, "data_%llu_%s.%s", NumOfPairs, Name, Extension);
	
	FILE* File = std::fopen(Temp, "wb");
	if(!File)
	{
		fprintf(stderr, "Unable to open \"%s\" for writing.\"", Temp);
	}
	
	return File;
}

static f64
RandomDegree(random_series* Series, f64 Center, f64 Radius, f64 MaxAllowed)
{
	f64 MinVal = Center - Radius;
	if(MinVal < -MaxAllowed)
	{
		MinVal = -MaxAllowed;
	}
	
	f64 MaxVal = Center + Radius;
	if(MaxVal > MaxAllowed)
	{
		MaxVal = MaxAllowed;
	}
	
	f64 Result = RandomInRange(Series, MinVal, MaxVal);
	return Result;
}

int mainProfiling(int ArgsCount,char** Args)
{
    
    if(ArgsCount == 1)
    {
        fprintf(stdout, "Usage: %s [random seed] [number of coordinate pairs to generate] [mode] \n", Args[0]);
        return 1;
    }
    
    u64 RandomSeed = 0;
    u64 NumberOfPairs = 0;
	bool bIsClusterMode = false;
	s64 PairsPerCluster = 0;
    for(int ArgsIndex = 1; ArgsIndex < ArgsCount; ++ArgsIndex)
    {
        char* Arg = Args[ArgsIndex];
        u64 ArgAsNumber = std::atoll(Arg);
        if(ArgsIndex == 1)
        {
            // Random Seed
            RandomSeed = ArgAsNumber;
        }
        else if(ArgsIndex == 2)
        {
            // Number of Pairs to Generate
            NumberOfPairs = ArgAsNumber;
        }
		else if(ArgsIndex == 3)
		{
			bIsClusterMode = !strcmp(Arg, "cluster");
			fprintf(stdout, "Using %s mode... \n", bIsClusterMode ? "Cluster" : "Uniform");
			
			if(!bIsClusterMode)
			{
				PairsPerCluster = U64Max;
			}
		}
        else
        {
            printf("Too much Args passed-in \n");
        }
    }
	
	if(ArgsCount < 4)
	{
		printf("Using Uniform Mode, not method specified.... \n");
	}
    
    // HARVESINE SUM LOGIC
    // Create a Json file format with pairs -->  "pairs: { [{x0:, y0: }, {x1:, y1:}] }"
	
	// CLUSTER - Every N pairs we are going to change the X and Y center.
	// UNIFORM - We are just going to generate X and Y centers every time.
    
    FILE* Json = Open(NumberOfPairs, "pairs", "json");
    FILE* Data = Open(NumberOfPairs, "data", "f64");
    
	if(!Json || !Data)
	{
		return -1;
	}
	
	random_series Series = Seed(RandomSeed);
	
	
    const f64 EarthRadius = 6372.8;
    f64 Sum = 0;
	f64 SumCoeff = 1.0f / (f64)NumberOfPairs;
	
	f64 XCenter = 0;
	f64 YCenter = 0;
	
	f64 XMax = 180;
	f64 XMin = -XMax;
	f64 YMax = 90;
	f64 YMin = -YMax;
	
	u64 ClusterMaxPairs = bIsClusterMode ? NumberOfPairs / 64 + 1 : U64Max;
	
	
	fprintf(Json, "{\"pairs\": [\n");
    for(u64 PairIndex = 0; PairIndex < NumberOfPairs; ++PairIndex)
    {
		if(PairsPerCluster-- == 0)
		{
			PairsPerCluster = ClusterMaxPairs;
			
			// Generate new X and Y centers
			XMin =(f64) RandomInRange(&Series, -180, 180);
			XMax = (f64)RandomInRange(&Series, -180, 180);
			XCenter = (f64)RandomInRange(&Series, XMin, XMax);
			
			YMin = (f64)RandomInRange(&Series, -90, 90);
			YMax = (f64)RandomInRange(&Series, -90, 90);
			YCenter = (f64)RandomInRange(&Series, YMin, YMax);
			
		}
		
        f64 X0 = RandomDegree(&Series, XCenter, XMin, XMax);
		f64 Y0 = RandomDegree(&Series, YCenter, YMin, YMax);
		f64 X1 = RandomDegree(&Series, XCenter, XMin, XMax);
		f64 Y1 = RandomDegree(&Series, YCenter, YMin, YMax);
		
		f64 HaversineDistance = ReferenceHaversine(X0, Y0, X1, Y1, 6372.8);
		Sum += SumCoeff*HaversineDistance;
		
		const char* JSONSep = PairIndex == NumberOfPairs - 1 ? "\n" : " ,\n";
		fprintf(Json, "    {\"x0\":%.16f, \"y0\":%.16f, \"x1\"%.16f, \"y1\":%.16f}%s", X0, Y0, X1, Y1, JSONSep);
        
        fwrite(&HaversineDistance, sizeof(HaversineDistance), 1, Data);
		
		// TODO The file with the Harvesine raw data in bytes.
    }
	fprintf(Json, "}");
	fclose(Json);
    
    std::cout << "Sum: " << Sum << std::endl;
	return 0;
}