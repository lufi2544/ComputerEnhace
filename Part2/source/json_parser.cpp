#include "types.h"
#include <fstream>
#include <cstdio>

#include "json_parser.h"


/*
  * Json Parser that is meant to have a Json file as input and then parse it to some object that we can extract values from.
*/



// TODO: ?



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

void SetFlag(u32& Flags, enum_parser_flags FlagToSet, bool V)
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
    memset(Buffer, '\0', *Size);
    *Size = 0;
}


json_category MakeJsonCategory(category_context *CategoryContext, json_value CategoryValue, enum_json_value_type Type)
{
    json_category result;
     u32 BufferSize = sizeof(char) * CategoryContext->Size + 1;
    char* CategoryKeyBuffer = (char*)malloc(BufferSize);
    //snprintf(CategoryKeyBuffer, BufferSize, "%s", CategoryContext->Key);
    memset(CategoryKeyBuffer, '\0', CategoryContext->Size + 1);
strncpy(CategoryKeyBuffer, CategoryContext->Key, CategoryContext->Size);


    result.Key = CategoryKeyBuffer;
    result.Value = CategoryValue;
    result.ValueType = Type;
    
    ResetBuffer(CategoryContext->Key, &CategoryContext->Size);
    
    return result;
}

void AddJsonCategory(json_object *Json, json_category *Category)
{
    Json->Size++;
    realloc(Json->Categories, Json->Size * sizeof(json_category));
}

json_object CreateJson(char* FileName)
{
    FILE* file = fopen(FileName, "rb");
    if(!file)
    {
        fprintf(stderr, "Could not open the JSON file...");
        return { };
    }
    
    
    fseek(file, 0, SEEK_END);
    u32 Size = ftell(file);
    rewind(file);
    char* Buffer = (char*)malloc(Size * sizeof(char) + 1);//TODO: Maybe checking if a +1 is needed here.
    Buffer[Size] = '\0';
    
    // Get the file size;
    fread(Buffer, sizeof(char), Size, file);
    Buffer[Size] = '\0';
    
    fclose(file);
    
    json_object Result;
    Result.Categories =(json_category*) malloc(sizeof(json_category) * Size);
    
    u32 Flags = 0;
    char TempBuffer[256];
    memset(TempBuffer, '\0', sizeof(TempBuffer));
    
    json_category TempCategory;
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
                TempCategory.bIsOpen = true;
                continue;
            }
            
            if(TempCategory.bIsOpen)
            {
                // Category is none, then we push category , this is the end of a String as Key
                
                // Push the TempString to make the Category Key
                SetFlag(Flags, enum_parser_flags::flag_CategoryOpen, false);
                category_context CategoryContext;
                CategoryContext.Key = TempBuffer;
                CategoryContext.Size = TempBufferSize;
                
                auto a = strlen(CategoryContext.Key);
                auto b = strlen(TempBuffer);
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
                    if(TempCategory.bIsOpen)
                    {
                        char* ValueString = (char*)malloc(sizeof(char) * TempBufferSize + 1);
                        ValueString[TempBufferSize] = '\0';
                        strncpy(ValueString, TempBuffer, TempBufferSize);
                        TempCategory.Value.String = ValueString;
                        
                        SetFlag(Flags, enum_parser_flags::flag_ValuePushed, true);
                    }
                    else
                    {
                        fprintf(stderr, "Value parsed but Category is NULL...");
                        return {};
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
                auto a = strlen(TempBuffer);
                int f = 0;
                int c = a;
                int d = 3;
                }
        }
        else if(Token == enum_json_token::token_Coma)
        {
            // TODO IMPLEMENT THE Bool, int categorization in the TempCategory.
            
            if(!TempCategory.bIsOpen)
            {
                fprintf(stderr, "No TempCategory when reaching to a coma...");
                return {};
            }
            
            // Optimization for the String.
            if(CheckFlag(Flags, enum_parser_flags::flag_ValuePushed))
            {
                continue;
            }
            
            if(strncmp(TempBuffer, "true", TempBufferSize))
            {
                TempCategory.ValueType = enum_json_value_type::type_Bool;
                TempCategory.Value.Bool = true;
            }
            else if(strncmp(TempBuffer, "false", TempBufferSize))
            {
                TempCategory.ValueType = enum_json_value_type::type_Bool;
                TempCategory.Value.Bool = false;
            }
            else
            {
                TempCategory.ValueType = enum_json_value_type::type_Number;
                TempCategory.Value.Number = atoll(TempBuffer);
            }
            
            TempCategory.bIsOpen = false;
            ResetBuffer(TempBuffer, &TempBufferSize);
            Result.Categories[Result.Size++] = TempCategory;
        }
        
    }
    
    
    return Result;
}