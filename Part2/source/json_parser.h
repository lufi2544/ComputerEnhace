/* date = November 27th 2024 6:46 pm */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H


enum enum_json_token
{
    token_None,
    token_Dpoints,
    token_OpenBraces,
    token_CloseBraces,
    token_OpenSquareBracket,
    token_CloseSquareBracket,
    token_Coma,
    token_Blank,
    token_LetterOrNumber,
    token_DQuote,
};

enum enum_parser_flags : u16
{
    flag_Open = 0,
    flag_completed = 1,
    flag_CategoryOpen = 2,
    flag_sub_CategoryOpen = 3, /*  when a category is opened and we are writing some categories on the inside. */
    flag_ValueOpen = 4,
    flag_ValuePushed = 5,
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

enum enum_json_value_type
{
    type_None,
    type_Number,
    type_Bool,
    type_String,
};


union json_value
{
    char* String = nullptr;
    float Number;
    bool Bool;
    u8 _pad[3];
};

struct json_category
{
    char* Key = nullptr;
    json_value Value;
    enum_json_value_type ValueType = type_None;
    bool bIsOpen = false;
};


struct json_object
{
    json_category* Categories = nullptr;
    s32 Size = 0;
};


json_object CreateJson(char* FileName);

#endif //JSON_PARSER_H
