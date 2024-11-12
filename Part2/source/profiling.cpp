
#include <iostream>
#include "harvesine_formula.cpp"

int main(char** Args, int ArgsCount)
{
    
    if(size == 0)
    {
        printf(stdout, "Usage: harvesine_generator_release.exe [random seed] [number of coordinate pairs to generate] \n");
        return 1;
    }
    
    u64 RandomSeed = 0;
    u64 NumberOfPairs = 0;
    for(int ArgsIndex = 0; ArgsIndex < ArgsCount; ++ArgsIndex)
    {
        char* Arg = Args[ArgsIndex];
        u64 ArgAsNumber = std::atoll(Arg);
        if(ArgsIndex == 0)
        {
            // Random Seed
            RandomSeed = ArgAsNumber;
        }
        else if(ArgsIndex == 1)
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
    
	return 0;
}