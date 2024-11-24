#include "types.h"


/*
  * Json Parser that is meant to have a Json file as input and then parse it to some object that we can extract values from.
*/



// TODO: ?


enum enum_json_character
{
    character_None,
    character_parenthesis,
    character_dpoints,
    character_open_braces,
    character_close_braces,
    character_open_bracket,
    character_close_square_bracket,
    character_coma,
    character_blank,
};


/*
* PLAN:
* Json will have some categories: inside we can have 
* - Each category can expand to more categories.
* - Each category can have an array of data with "[]".
* - Either a list of elements that can be composed by different categories between "[]" which have the same categories, [{type: dog, age: 21}, {type: cat, age: 44}]. Elements from the same type.
* - A simple list of categories". - "general": { age: 21, type: person }
* - All the Json has to be between "{}";


* -> Hierarchical Categories: You can expand categories infinitely with nesting.
* -> Arrays for Homogeneous Data: Arrays [] are used for lists of objects of the same type (or with consistent properties).
* -> Curly Braces for Objects: Use {} for key-value pairs representing entities.
*/


/*
 JsonValueAsCategory { JsonObject*; size; }
JsonValueAsArray { JsonObject*; size } ["hello", "hello1"...] or [{"type": "hello", "age": 21}], [1, 1, 3, 3]
JsonAsValueList{ {"type": "hero",  "age": 22}, "type": "withch", "age": 10101}

JsonValueAsString{ char*; size } 
 JsonValueAsFloat { float }
JsonValueAsBool { bool }

*/

/* Output from the parsing. */
struct json_object
{
    
};