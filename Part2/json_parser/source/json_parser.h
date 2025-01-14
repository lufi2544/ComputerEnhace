/* date = November 27th 2024 6:46 pm */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "types.h"

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
    token_Minus,
};

enum enum_parser_flags : u16
{
    flag_Open = 0,
    flag_completed = 1,
    flag_CategoryOpen = 2,
    flag_sub_CategoryOpen = 3, /*  when a category is opened and we are writing some categories on the inside. */
    flag_ValueOpen = 4,
    flag_ValuePushed = 5,
    flag_String_Opened = 6,
    flag_Array_Opened = 7,
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
    type_Json,
    type_Array,
};

union json_array_value
{
    char** StringArray;
    float* NumberArray;
    bool* BoolArray;
    struct json_object** JsonObjectArray;
};

struct json_array
{
    json_array_value Value;
    enum_json_value_type Type;
     u32 Size;
};

// TODO figure out a way of declaring the json_object with a type value in stead of a ptr.
union json_value
{
    char* String;
    float Number;
    bool Bool;
    struct json_object* Json;
    json_array JsonArray;
};

struct json_category
{
    char* Key = nullptr;
    json_value Value;
    enum_json_value_type ValueType = type_None;
    
    void Release()
    {
        if(Key)
        {
            free(Key);
            Key = nullptr;
        }
        if (ValueType == type_String)
        {
            free(Value.String);
            Value.String = nullptr;
        }
        else if(ValueType == type_Json)
        {
            delete Value.Json;
        }
        else if(ValueType == type_Array)
        {
            json_array& ArrayRef = Value.JsonArray;
            switch (ArrayRef.Type)
            {
                case enum_json_value_type::type_String :
                {
                    for(int i = 0; i < ArrayRef.Size; ++i)
                    {
                        char* String = ArrayRef.Value.StringArray[i];
                        free(String);
                    }
                    
                    free(ArrayRef.Value.StringArray);
                }
                break;
                
                case enum_json_value_type::type_Number :
                {
                    free(ArrayRef.Value.NumberArray);
                }
                break;
                
                case enum_json_value_type::type_Bool :
                {
                    free(ArrayRef.Value.BoolArray);
                }
                break;
                
                case enum_json_value_type::type_Json :
                {
                    for(int i = 0; i < ArrayRef.Size; ++i)
                    {
                        json_object* Json = ArrayRef.Value.JsonObjectArray[i];
                        delete(Json);
                    }
                    
                    delete (ArrayRef.Value.JsonObjectArray);
                }
                break;
                
                default:
                break;
            }
        }
    }
};


struct TempArrayContext
{
    u32 Size = 0;
    json_array_value Array;
};

struct temp_array_data
{
    TempArrayContext ArrayBoolContext;
    TempArrayContext ArrayStringContext;
    TempArrayContext ArrayNumberContxt;
    TempArrayContext ArrayJsonContext;

    enum_json_value_type Type = type_None;
    u32 Size = 0;
};

// Maybe making a different struct for this object, so we can have a handler and a json 
// because in the subcategory is a bit weird.
struct json_object
{
    
    json_object() = default;
    json_object(const char* JsonFileName);
    
    ~json_object()
    {
        for(int i = 0; i < Size; ++i)
        {
            json_category Category = Categories[i];
            Category.Release();
        }
        
        delete[] Categories;
        Categories = nullptr;
    }
    
    
    u32 ParseBuffer(const char* Buffer, u32 BufferSize, u32 FirstIndex = 0);
    void PushJsonCategory(json_category *Category);
    void Print();
    
    
    private:
    void PushArrayValue(temp_array_data* ArrayData, u16 Flags, char* TempBuffer, u32* TempBufferSize);
    void PushArray(json_category* TempCategory, temp_array_data* ArrayData);
    
    public:
    const char* Name = nullptr;
    json_category* Categories = nullptr;
    s32 Size = 0;
};





#endif //JSON_PARSER_H
