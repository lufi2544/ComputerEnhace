
#include <iostream>
#include "harvesine_formula.cpp"
#include <random>


void
GetRandomCoords(u64 Seed, s64& X0, s64& Y0, s64& X1, s64& Y1)
{
    std::mt19937 generator(Seed);
    std::uniform_int_distribution<s64> Distribution_X(-180, 180);
    
    s64 random_number_X0 = Distribution_X(generator);
    s64 random_number_X1 = Distribution_X(generator);
    
    std::uniform_int_distribution<s64> Distribution_Y(-90, 90);
    
    s64 random_number_Y0 = Distribution_Y(generator);
    s64 random_number_Y1 = Distribution_Y(generator);
    
    printf("Random Number: X0: %+lld X1: %+lld Y1: %+lld Y1: %+lld \n", random_number_X0, random_number_X1, random_number_Y0, random_number_Y1);
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
    
    // Generate 4 numbers randomly and pass them to the function.
    s64 X0, X1, Y0, Y1;
    
    GetRandomCoords(120398711, X0, X1, Y0, Y1);
    
    
    
	return 0;
}