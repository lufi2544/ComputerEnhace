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
            || (Character > 96 /* ` */ && Character < 123 /* { */) 
            || (Character == 46)
            || ((Character > 47) && (Character < 58)) /* Numbers */
            || (Character == '.'))
    {
        // Defining if we have a letter of the symbol _ which we accept.
        return enum_json_token::token_LetterOrNumber;
    }
    
    return enum_json_token::token_None;
}

bool CheckFlag(u16 Flags, enum_parser_flags FlagToCheck)
{
    return Flags & (1 << static_cast<u16>(FlagToCheck));
}

void SetFlag(u16& Flags, enum_parser_flags FlagToSet, bool V)
{
    if(V)
    {
        Flags |= (1 << static_cast<u16>(FlagToSet));
    }
    else
    {
        
        Flags &= (~(1 << static_cast<u16>(FlagToSet)));
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

char* MakeBuffer(u32 Size)
{
    char* result = (char*)malloc(sizeof(char) * Size + 1);
    memset(result,'\0', Size + 1);
    
    return result;
}

char* MakeBufferCopy(char *Buffer, u32 BufferSize)
{
    char* result = MakeBuffer(BufferSize);
    strncpy(result, Buffer, BufferSize);
    
    return result;
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

void PushChar(char V, char *Buffer, u32& Size)
{
    Buffer[Size++] = V;
}

void EvaluateCategory(json_category *Category, char *Buffer, u32 *BufferSize)
{
    if(Category->ValueType == enum_json_value_type::type_String)
    {
        Category->Value.String = MakeBufferCopy(Buffer, *BufferSize);
    }
    else if(strncmp(Buffer, "true", *BufferSize) == 0)
    {
        Category->ValueType = enum_json_value_type::type_Bool;
        Category->Value.Bool = true;
    }
    else if(strncmp(Buffer, "false", *BufferSize) == 0)
    {
        Category->ValueType = enum_json_value_type::type_Bool;
        Category->Value.Bool = false;
    }
    else
    {
        Category->ValueType = enum_json_value_type::type_Number;
        Category->Value.Number = atoll(Buffer);
    }
    
    ResetBuffer(Buffer, BufferSize);
    
}

void json_object::PushJsonCategory(json_category *Category)
{
    Categories[Size++] = *Category;
    *Category = json_category{};
}

json_object::json_object(const char* FileName)
{
    FILE* file = fopen(FileName, "rb");
    if(!file)
    {
        fprintf(stderr, "Could not open the JSON file...");
        return;
    }
    
    
    fseek(file, 0, SEEK_END);
    u32 Size = ftell(file);
    rewind(file);
    char* Buffer = MakeBuffer(Size);
    // Get the file size;
    fread(Buffer, sizeof(char), Size, file);
    fclose(file);
    
    Name = FileName;
    Categories =(json_category*) malloc(sizeof(json_category) * Size);
    
    u16 Flags = 0;
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
            // We have something in the buffer.
            if(TempBufferSize > 0)
            {
                if(TempCategory.Key)
                {
                    // Value Buffer as String.
                    TempCategory.ValueType = enum_json_value_type::type_String;
                }
                else
                {
                    // Category Buffer
                    category_context CategoryContext;
                    CategoryContext.Key = TempBuffer;
                    CategoryContext.Size = TempBufferSize;
                    
                    TempCategory = MakeJsonCategory(&CategoryContext, json_value{}, enum_json_value_type::type_None);
                    ResetBuffer(TempBuffer, &TempBufferSize);
                }
                
            }
        }
        else if(Token == enum_json_token::token_Dpoints)
        {
            // Starting a Value here.
            SetFlag(Flags, enum_parser_flags::flag_ValueOpen, true);
        }
        else if(Token == enum_json_token::token_LetterOrNumber)
        {
            PushChar(BufferChar, TempBuffer, TempBufferSize);
        }
        else if(Token == enum_json_token::token_Coma || Token == enum_json_token::token_CloseBraces)
        {
            // CHANGING THE CATEGORY
            
            EvaluateCategory(&TempCategory, TempBuffer, &TempBufferSize);
            PushJsonCategory(&TempCategory);
        }
    }
    
}

void json_object::Print()
{
    printf("Printing Json: %s \n", Name);
    for(int i = 0; i < Size; ++i)
    {
        json_category Category = Categories[i];
        
        char Buffer[32];
        memset(Buffer, '\0', 32);
        
        json_value V = Category.Value;
        if(Category.ValueType == enum_json_value_type::type_String)
        {
            strncpy(Buffer, V.String, strlen(V.String));
        }
        else if(Category.ValueType == enum_json_value_type::type_Number)
        {
            snprintf(Buffer, sizeof(Buffer), "%.2f", V.Number);
        }
        else if(Category.ValueType == enum_json_value_type::type_Bool)
        {
            if(V.Bool)
            {
                snprintf(Buffer, sizeof(Buffer), "true");
            }
            else
            {
                snprintf(Buffer, sizeof(Buffer), "false");
            }
        }
        
        printf("%s : %s \n", Category.Key, Buffer);
    }
}