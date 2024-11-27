#include "types.h"
#include <fstream>
#include <cstdio>


/*
  * Json Parser that is meant to have a Json file as input and then parse it to some object that we can extract values from.
*/



// TODO: ?


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
    token_Letter,
    token_DQuote,
};

enum enum_parser_flags : u32
{
    flag_Open,
    flag_completed,
    flag_CategoryOpen,
    flag_sub_CategoryOpen, /*  when a category is opened and we are writing some categories on the inside. */
    flag_ValueOpen,
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
    s32 Number;
    bool Bool;
    char* String = nullptr;
};

struct json_category
{
    char* Key = nullptr;
    json_value Value;
    enum_json_value_type ValueType = type_None;
};


struct json_object
{
    json_category* Categories = nullptr;
    s32 Size = 0;
};

enum_json_token GetToken(char Character)
{
    if(Character == ' ')
    {
        return enum_json_token::token_Blank;
    }
    else if(Character == ':')
    {
        return enum_json_token::token_Dpoints;
    }
    else if(Character == '{')
    {
        return enum_json_token::token_OpenBraces;
    }
    else if(Character == '}')
    {
        return enum_json_token::token_CloseBraces;
    }
    else if(Character == '[')
    {
        return enum_json_token::token_OpenSquareBracket;
    }
    else if(Character == ']')
    {
        return enum_json_token::token_CloseSquareBracket;
    }
    else if(Character == ',')
    {
        return enum_json_token::token_Coma;
    }
    else if (Character == '"')
    {
        return enum_json_token::token_DQuote;
    }
    else if((Character > 64/* @ */ && Character < 91 /* [  */) 
            || (Character == 95/* _ */) 
            || (Character > 96 /* ` */ && Character < 123 /* { */) )
    {
        // Defining if we have a letter of the symbol _ which we accept.
        return enum_json_token::token_Letter;
    }
    
    return enum_json_token::token_None;
}

bool CheckFlag(u32 Flags, enum_parser_flags FlagToCheck)
{
    return Flags & (1 << static_cast<u32>(FlagToCheck));
}

void SetFlag(u32 Flags, enum_parser_flags FlagToSet, bool V)
{
    if(V)
    {
        Flags |= (1 << static_cast<u32>(FlagToSet));
    }
    else
    {

        Flags &= (~(1 << static_cast<u32>(FlagToSet)));
    }
}


struct category_context
{
    char* Key = nullptr;
    u32 Size = 0;
};


void ResetBuffer(char* Buffer, u32 *Size)
{
    for(u32 i = 0; i < *Size; ++i)
    {
        Buffer[i] = ' ';
    }
    
    *Size = 0;
}


json_category* MakeJsonCategory(category_context *CategoryContext, json_value CategoryValue, enum_json_value_type Type)
{
    json_category* result = (json_category*)malloc(sizeof(json_category));
    char* CategoryKeyBuffer = (char*)malloc(sizeof(char) * CategoryContext->Size + 1);
    strncpy(result->Key, CategoryContext->Key, CategoryContext->Size);
    
    result->Key[CategoryContext->Size] = '\0';
    ResetBuffer(CategoryContext->Key, &CategoryContext->Size);
    
    result->Value = CategoryValue;
    result->ValueType = Type;
    
    
    return result;
}

void AddJsonCategory(json_object *Json, json_category *Category)
{
    Json->Size++;
    realloc(Json->Categories, Json->Size * sizeof(json_category));
}

json_object* CreateJson(char* FileName)
{
    FILE* file = fopen(FileName, "r");
    if(!file)
    {
        fprintf(stderr, "Could not open the JSON file...");
        return nullptr;
    }
    
    
    fseek(file, 0, SEEK_END);
    u32 Size = ftell(file);
    char* Buffer = (char*)malloc(Size * sizeof(char) + 1);//TODO: Maybe checking if a +1 is needed here.
    Buffer[Size] = '\0';
    
    // Get the file size;
    fgets(Buffer, Size, file);
    fclose(file);
    
    json_object* Json = (json_object*)malloc(sizeof(json_object));
    
    u16 Flags = 0;
    char TempBuffer[256];
    TempBuffer[255] = '\0';
    memset(TempBuffer, ' ', 255);
    
    json_category* TempCategory = nullptr;
    u32 TempBufferSize = 0;
    for(u32 Index = 0; Index < Size; ++Index)
    {
        char BufferChar = Buffer[Index];
        enum_json_token Token = GetToken(BufferChar);
        
        if(Token == enum_json_token::token_Blank)
        {
            continue;
        }
        
        // Check if we have the first "{" of the Json
        if(Token == enum_json_token::token_OpenBraces)
        {
            if(!CheckFlag(Flags, enum_parser_flags::flag_Open))
            {
            // Opening the Json
            SetFlag(Flags, enum_parser_flags::flag_Open, true);
            }
            else
            {
                // Handle SubCategory.
            }
        }
            else if(Token == enum_json_token::token_DQuote)
        {
            // Finishing a Category Key. For now, this means that a Category String has finished, could be the Key of a Category or
            // the Value as String.
            
            
            if(!CheckFlag(Flags, enum_parser_flags::flag_CategoryOpen))
            {
                SetFlag(Flags, enum_parser_flags::flag_CategoryOpen, true);
                continue;
            }
            
            if(TempCategory == nullptr)
            {
                // Category is none, then we push category , this is the end of a String as Key
                
                // Push the TempString to make the Category Key
                SetFlag(Flags, enum_parser_flags::flag_CategoryOpen, false);
                category_context CategoryContext;
                CategoryContext.Key = TempBuffer;
                CategoryContext.Size = TempBufferSize;
                
                TempCategory = MakeJsonCategory(&CategoryContext, json_value{}, enum_json_value_type::type_None);
                ResetBuffer(TempBuffer, &TempBufferSize);
                
            }
            else
            {
                if(!CheckFlag(Flags, enum_parser_flags::flag_ValueOpen))
                {
                    // Begining of a Value
                    SetFlag(Flags, enum_parser_flags::flag_ValueOpen, true);
                    continue;
                }
                else
                {
                    // End of a value as string, only case we close the value here.
                    SetFlag(Flags, enum_parser_flags::flag_ValueOpen, false);
                    if(TempCategory)
                    {
                        char* ValueString = (char*)malloc(sizeof(char) * TempBufferSize + 1);
                        ValueString[TempBufferSize] = '\0';
                        strncpy(ValueString, TempBuffer, TempBufferSize);
                        TempCategory->Value.String = ValueString;
                        ResetBuffer(TempBuffer, &TempBufferSize);
                    }
                    else
                    {
                        fprintf(stderr, "Value parsed but Category is NULL...");
                        return nullptr;
                    }
                }
            }
            
        }
        else if(Token == enum_json_token::token_Dpoints)
        {
            // Starting a Value here.
            SetFlag(Flags, enum_parser_flags::flag_ValueOpen, true);
        }
            else if(Token == enum_json_token::token_Letter)
            {
                // Json is Opened, proceed with the category
                if(CheckFlag(Flags, enum_parser_flags::flag_CategoryOpen))
                {
                TempBuffer[TempBufferSize++] = BufferChar;
                }
        }
        else if(Token == enum_json_token::token_Coma)
        {
            // TODO IMPLEMENT THE Bool, int categorization in the TempCategory.
        }
        
    }
    
    
    return nullptr;
}