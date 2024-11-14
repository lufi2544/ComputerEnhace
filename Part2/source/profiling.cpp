
#include <iostream>
#include "harvesine_formula.cpp"
#include <random>
#include <cstdio>


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
    
    /*
JSON FILES: 
Java Script Object Notation - easy understandable by humans and easy to implement in different languages

TArray<TMap<String, TArray<TMap<String, String>>>>

- It is between "{}"
- We could have different Categories -> 
 {
 "user": { ...  },
"tasks": { ...   }
}

- And different Items inside each Category, that can be an Object Item or an Array Item

Object:

{
 "user": 
{ 
"id": 12324,
"name": "Juanes",
"age": 12039812309487
},


}

Array: category has a [] for the items.

{
 "user": 
{ 
"id": 12324,
"name": "Juanes",
"age": 12039812309487
},

"tasks": { [
{
"id": 13
"name": LOLO
"age": 123
},
{
"id": 13,
"name": "LOLO1",
"age": 1209123
}
]
}
*/
    
    
    
    
    FILE* json = std::fopen("data_coords.json", "w");
    
    const f64 EarthRadius = 6372.8;
    
    std::fprintf(json, "{ \n");
    std::fprintf(json, "pairs: [ \n");
    f64 Sum = 0;
    for(u64 PairIndex = 0; PairIndex < NumberOfPairs; ++PairIndex)
    {
        // Generate 4 numbers randomly and pass them to the function.
        f64 X0 = 0;
        f64 X1 = 0;
        f64 Y0 = 0;
        f64 Y1 = 0;
        RandomCoords(120131234123, X0, X1, Y0, Y1);
        
        Sum += ReferenceHaversine(X0, X1, Y0, Y1, EarthRadius);
        std::fprintf(json, "     {\"X0:\" %.6f, \"X1:\" %.6f, \ Y0:\" %.6f, \"Y1:\" %.6f}", X0, X1, Y0, Y1);
        if(PairIndex < NumberOfPairs - 1)
        {
            std::fprintf(json, ",");
        }
        
        std::fprintf(json, "\n");
    }
    
    std::fprintf(json, "]");
    std::fprintf(json, "\n");
    std::fprintf(json, "}");
    std::fclose(json);
    
    std::cout << "Sum: " << Sum << std::endl;
    
    
    
	return 0;
}