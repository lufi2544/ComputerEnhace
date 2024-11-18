
#include <iostream>
#include "harvesine_formula.cpp"
#include <random>
#include <cstdio>


#define _CRT_SECURE_NO_WARNINGS
#define U64Max  UINT64_MAX


struct random_series
{
	u64 A, B, C, D;
};

u64
RotateLeft(u64 Value, int Times)
{
	u64 Result = (Value << Times) | (Value >> (64 - Times)); // Wrapping around the bits 10111111 << 3 | 10111111 >> (8 - 3) = 11111101.
	return Result;
}

u64
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

random_series
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

f64 RandomInRange(random_series* Series, f64 Min, f64 Max)
{
	int a  = 0;
	f64 t = (f64)RandomU64(Series) / (f64)U64Max;
	f64 Result = ((1 - t) * Min) + (t * Max);
	
	return Result;
}

void
RandomCoords(u64 Seed, f64& X0, f64& Y0, f64& X1, f64& Y1)
{
    std::mt19937 generator(Seed);
    std::uniform_int_distribution<s64> Distribution_X(-180, 180);
    
    X0 = Distribution_X(generator);
    X1 = Distribution_X(generator);
    
    std::uniform_int_distribution<s64> Distribution_Y(-90, 90);
    
    Y0 = Distribution_Y(generator);
    Y1 = Distribution_Y(generator);
    
    
}

FILE*
Open(const char* Name, const char* Extension)
{
	char Temp[256];
	sprintf(Temp, "%s.%s", Name, Extension);
	
	FILE* File = std::fopen(Temp, "wb");
	if(!File)
	{
		fprintf(stderr, "Unable to open \"%s\" for writing.\"", Temp);
	}
	
	return File;
}

f64
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

int main(int ArgsCount,char** Args)
{
    
    if(ArgsCount == 1)
    {
        fprintf(stdout, "Usage: %s [random seed] [number of coordinate pairs to generate] \n", Args[0]);
        return 1;
    }
    
    u64 RandomSeed = 0;
    u64 NumberOfPairs = 0;
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
        else
        {
            printf("Too much Args passed-in \n");
        }
    }
    
    // HARVESINE SUM LOGIC
    // Create a Json file format with pairs -->  "pairs: { [{x0:, y0: }, {x1:, y1:}] }"
    
    FILE* Json = Open("data", "json");
    
	if(!Json)
	{
		return -1;
	}
	
	random_series Series = Seed(RandomSeed);
	
	
    const f64 EarthRadius = 6372.8;
    f64 Sum = 0;
	f64 SumCoeff = 1.0f / (f64)NumberOfPairs;
	f64 XCenter = 0;
	f64 YCenter = 0;
	
	fprintf(Json, "{\"pairs\": [\n");
    for(u64 PairIndex = 0; PairIndex < NumberOfPairs; ++PairIndex)
    {
        f64 X0 = RandomDegree(&Series,XCenter,  180, -180);
		f64 Y0 = RandomDegree(&Series, YCenter, 90, -90);
		f64 X1 = RandomDegree(&Series, XCenter, 180, -180);
		f64 Y1 = RandomDegree(&Series, YCenter, 90, -90);
		
		f64 HaversineDistance = ReferenceHaversine(X0, Y0, X1, Y1, 6372.8);
		Sum += SumCoeff*HaversineDistance;
		
		const char* JSONSep = PairIndex == NumberOfPairs - 1 ? "\n" : " ,\n";
		fprintf(Json, "    {\"x0\":%.16f, \"y0\":%.16f, \"x1\"%.16f, \"y1\":%.16f}%s", X0, Y0, X1, Y1, JSONSep);
		
		// TODO The file with the Harvesine raw data in bytes.
    }
	fprintf(Json, "}");
	fclose(Json);
    
    std::cout << "Sum: " << Sum << std::endl;
    
    
    
	return 0;
}