#include "types.h"
#include <fstream>
#include <cstdio>

#include "json_parser.h"


/*
  * Json Parser that is meant to have a Json file as input and then parse it to some object that we can extract values from.
*/

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

void EvaluateCategory(json_category *Category, char *TempBuffer, u32 *TempBufferSize)
{
    if(Category->ValueType == enum_json_value_type::type_None)
    {
        ResetBuffer(TempBuffer, TempBufferSize);
        return;
    }
    
    if(Category->ValueType == enum_json_value_type::type_String)
    {
        Category->Value.String = MakeBufferCopy(TempBuffer, *TempBufferSize);
    }
    else if((*TempBufferSize > 0) && strncmp(TempBuffer, "true", *TempBufferSize) == 0)
    {
        Category->ValueType = enum_json_value_type::type_Bool;
        Category->Value.Bool = true;
    }
    else if((*TempBufferSize > 0) && strncmp(TempBuffer, "false", *TempBufferSize) == 0)
    {
        Category->ValueType = enum_json_value_type::type_Bool;
        Category->Value.Bool = false;
    }
    else if(Category->ValueType == enum_json_value_type::type_Number)
    {
        Category->ValueType = enum_json_value_type::type_Number;
        Category->Value.Number = atoll(TempBuffer);
    }
    
    ResetBuffer(TempBuffer, TempBufferSize);
}

void json_object::PushJsonCategory(json_category *Category)
{
    if(Category->ValueType != enum_json_value_type::type_None)
    {
        u32 TempSize = Size;
        Categories = (json_category*)realloc(Categories, ++TempSize * sizeof(json_category));
        Categories[Size++] = *Category;
        *Category = json_category{};
    }
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
    
    ParseBuffer(Buffer, Size, 0);
}

void json_object::Print()
{
    
    for(int i = 0; i < Size; ++i)
    {
        json_category Category = Categories[i];
        json_value V = Category.Value;
        if(Category.ValueType == enum_json_value_type::type_Json)
        {
            printf("%s :\n", Category.Key);
            printf("{ \n");
            V.Json->Print();
            printf("} \n");
        }
        else if(Category.ValueType == enum_json_value_type::type_Array)
        {
            printf("%s: \n", Category.Key);
            printf("[ ");
            json_array& ArrayRef = Category.Value.JsonArray;
            
            if(ArrayRef.Type == type_Number)
            {
            for(int i = 0; i < ArrayRef.Size; ++i)
            {
                // TODO TYPE CHECK HERE
                float Num = ArrayRef.Value.NumberArray[i];
                
                if(i < ArrayRef.Size - 1)
                {
                printf("%.2f , ", Num);
                }
                else
                {
                    printf("%.2f", Num);
                }
            }
            }
            else if(ArrayRef.Type == type_String)
            {
                for(int i = 0; i < ArrayRef.Size; ++i)
                {
                    char* String = ArrayRef.Value.StringArray[i];
                    if(i < ArrayRef.Size - 1)
                    {
                        printf("%s ,", String);
                    }
                    else
                    {
                        printf("%s", String);
                    }
                }
            }
            
            
            printf(" ]\n");
        }
        else
        {
            char Buffer[32];
            memset(Buffer, '\0', ArrayCount(Buffer));
            
            
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
}

enum class enum_temp_type
{
    type_None,
    type_String,
    type_Bool,
    type_Float,
        type_Json,
};


void EvaluateArrayValue(u16 Flags, enum_json_value_type *TempType, char* TempBuffer, u32 TempBufferSize)
{
    if((strncmp(TempBuffer, "true", TempBufferSize) == 0) || (strncmp(TempBuffer, "false", TempBufferSize) == 0))
    {
        *TempType = type_Bool;
    }
    else if(CheckFlag(Flags, enum_parser_flags::flag_String_Opened))
    {
        *TempType = type_String;
    }
    else
    {
        // TODO EXCLUDE WHEN HAVING A JSON
        *TempType = type_Number;
    }
    
}

u32 json_object::ParseBuffer(const char* Buffer, u32 BufferSize, u32 FirstIndex /*= 0*/)
{
    u32 ReadChars = 0;
    u16 Flags = 0;
    
    // TEMP DATA
    char TempBuffer[256];
    memset(TempBuffer, '\0', sizeof(TempBuffer));
    
     enum_json_value_type TempType = type_None;
    bool* TempBoolArray = nullptr;
    char** TempStringArray = nullptr;
    f32* TempFloatArray = nullptr;
    bool bTempDataFree = false;
    
    u8 InitialTempData = 100;
    u32 CurrentTempDataBufferSize = 0;
    u8 TempArraySize = 0;
    
    
    json_category TempCategory;
    u32 TempBufferSize = 0;
    for(u32 Index = FirstIndex; Index < BufferSize; ++Index)
    {
        ++ReadChars;
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
                // Adding another Json as a subcategory
                if(TempCategory.Key)
                {
                    TempCategory.ValueType = enum_json_value_type::type_Json;
                }
                
                // Handle SubCategory.
                // Here we have another category, so we have to create a json_object
                //NOTE: for now lets handle a simple category.
                
                //TODO: MEMORY!!! Is this needed?
                json_object* SubJson = (json_object*)malloc(sizeof(json_object));
                new (SubJson) json_object();
                
                u32 SubJsonReadChars = SubJson->ParseBuffer(Buffer, BufferSize, Index + 1);
                Index += SubJsonReadChars;
                TempCategory.Value.Json = SubJson;
                PushJsonCategory(&TempCategory);
            }
        }
        else if(Token == enum_json_token::token_DQuote)
        {
            
            // We have something in the buffer. Finished the String Value
            if(TempBufferSize > 0)
            {
                SetFlag(Flags, enum_parser_flags::flag_String_Opened, false);
                if(TempCategory.Key)
                {
                    // Array Check
                    if(!CheckFlag(Flags, enum_parser_flags::flag_Array_Opened))
                    {
                    // Value Buffer as String.
                    TempCategory.ValueType = enum_json_value_type::type_String;
                    }
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
            else
            {
                // Start to read a String value
                SetFlag(Flags, enum_parser_flags::flag_String_Opened, true);
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
            
            // We are doing this all the time we find a letter or number, maybe figure out a way of set this once.FLAG?
            if(!CheckFlag(Flags, enum_parser_flags::flag_String_Opened)
               && !CheckFlag(Flags, enum_parser_flags::flag_Array_Opened))
            {
                TempCategory.ValueType = enum_json_value_type::type_Number;
            }
            
        }
        else if((!CheckFlag(Flags, enum_parser_flags::flag_Array_Opened)) && (Token == enum_json_token::token_Coma || Token == enum_json_token::token_CloseBraces))
        {
            // CHANGING THE CATEGORY
            
            EvaluateCategory(&TempCategory, TempBuffer, &TempBufferSize);
            PushJsonCategory(&TempCategory);
        }
        else if(Token == enum_json_token::token_OpenSquareBracket)
        {
            SetFlag(Flags, enum_parser_flags::flag_Array_Opened, true);
            TempCategory.ValueType = enum_json_value_type::type_Array;
        }
        else if(Token == enum_json_token::token_CloseSquareBracket)
        {
            SetFlag(Flags, enum_parser_flags::flag_Array_Opened, false);
            
            // PUSH THE LAST VALUE
            
                EvaluateArrayValue(Flags, &TempType, TempBuffer, TempBufferSize);
                switch(TempType)
                {
                    case enum_temp_type::type_Float:
                    {
                        if((TempArraySize + 1) > (CurrentTempDataBufferSize))
                        {
                            CurrentTempDataBufferSize += InitialTempData;
                            TempFloatArray = (f32*)realloc(TempFloatArray, (CurrentTempDataBufferSize) * sizeof(f32));
                        }
                        
                        TempFloatArray[TempArraySize++] = atoll(TempBuffer);
                    }
                    break;
                    
                    case enum_temp_type::type_Bool:
                    {
                        if((TempArraySize + 1) > (CurrentTempDataBufferSize))
                        {
                            CurrentTempDataBufferSize += InitialTempData;
                            TempBoolArray  = (bool*)realloc(TempBoolArray, (CurrentTempDataBufferSize) * sizeof(bool));
                        }
                        
                        bool bValueToAdd = !(strncmp(TempBuffer, "false", TempBufferSize) == 0);
                        TempBoolArray[TempArraySize++] = bValueToAdd;
                    }
                    break;
                    
                    case enum_temp_type::type_String:
                    {
                        if((TempArraySize + 1) > (CurrentTempDataBufferSize))
                        {
                            CurrentTempDataBufferSize += InitialTempData;
                            TempStringArray  = (char**)realloc(TempStringArray, (CurrentTempDataBufferSize) * sizeof(char*));
                        }
                    
                    char* String = (char*)malloc(sizeof(char) * TempBufferSize + 1);
                    memset(String, '\0', TempBufferSize + 1);
                    memcpy(TempStringArray[TempArraySize++], TempBuffer, TempBufferSize);
                    }
                    break;
                    
                    case enum_temp_type::type_Json:
                    {
                        // TODO IMPLEMENT JSON AS ARRAY
                    }
                    break;
                    
                    default: break;
                }
                
                ResetBuffer(TempBuffer, &TempBufferSize);
            
            json_array& ArrayRef = TempCategory.Value.JsonArray;
            ArrayRef.Size = TempArraySize;
            switch(TempType)
            {
                case enum_temp_type::type_Float:
                {
                    u32 Bytes = sizeof(f32) * ArrayRef.Size;
                    f32* AsNumbers = (f32*)malloc(Bytes);
                    memcpy(AsNumbers, TempFloatArray, Bytes);
                    ArrayRef.Value.NumberArray = AsNumbers;
                }
                break;
                
                case enum_temp_type::type_Bool:
                {
                    u32 Bytes = sizeof(bool) * ArrayRef.Size;
                    bool* AsBools = (bool*)malloc(Bytes);
                    memcpy(AsBools, TempBoolArray, Bytes);
                    ArrayRef.Value.BoolArray = AsBools;
                }
                break;
                
                case enum_temp_type::type_String:
                {
                    u32 Bytes = sizeof(char*) * ArrayRef.Size;
                    char** AsString = (char**)malloc(Bytes);
                    
                    for(int i = 0; i < ArrayRef.Size; ++i)
                    {
                        char* String = TempStringArray[i];
                        u32 StringToCpySize = strlen(String) + 1;
                        char* DestString = (char*)malloc(sizeof(char)* StringToCpySize);
                        memcpy(DestString, String, StringToCpySize);
                        
                        AsString[i] = DestString;
                    }
                    
                    memcpy(AsString, TempStringArray, Bytes);
                    ArrayRef.Value.StringArray = AsString;
                }
                break;
                
                case enum_temp_type::type_Json:
                {
                    // TODO IMPLEMENT JSON AS ARRAY
                }
                break;
                
                default: break;
            }
            
            
            // Reseting the ArraySize to reuse it.
             TempArraySize = 0;
            
        }
        else if(Token == enum_json_token::token_Coma)
        {
            // we are reading through the Array
            if(CheckFlag(Flags, enum_parser_flags::flag_Array_Opened))
            {
                EvaluateArrayValue(Flags, &TempType, TempBuffer, TempBufferSize);
                
                switch(TempType)
                {
                    case enum_temp_type::type_Float:
                    {
                        if((TempArraySize + 1) > (CurrentTempDataBufferSize))
                        {
                            CurrentTempDataBufferSize += InitialTempData;
                            TempFloatArray = (f32*)realloc(TempFloatArray, (CurrentTempDataBufferSize) * sizeof(f32));
                        }
                        
                        TempFloatArray[TempArraySize++] = atoll(TempBuffer);
                    }
                    break;
                    
                    case enum_temp_type::type_Bool:
                    {
                        if((TempArraySize + 1) > (CurrentTempDataBufferSize))
                        {
                        CurrentTempDataBufferSize += InitialTempData;
                            TempBoolArray  = (bool*)realloc(TempBoolArray, (CurrentTempDataBufferSize) * sizeof(bool));
                        }
                        
                        bool bValueToAdd = !(strncmp(TempBuffer, "false", TempBufferSize) == 0);
                            TempBoolArray[TempArraySize++] = bValueToAdd;
                    }
                    break;
                    
                    case enum_temp_type::type_String:
                    {
                        if((TempArraySize + 1) > (CurrentTempDataBufferSize))
                        {
                            CurrentTempDataBufferSize += InitialTempData;
                            TempStringArray  = (char**)realloc(TempStringArray, (CurrentTempDataBufferSize) * sizeof(char*));
                        }
                        
                        char* NewString = (char*)malloc(sizeof(char) * (TempBufferSize + 1));
                        memset(NewString, '\0', TempBufferSize + 1);
                        memcpy(NewString, TempBuffer, TempBufferSize);
                        TempStringArray[TempArraySize++] = NewString;
                    }
                    break;
                    
                    case enum_temp_type::type_Json:
                    {
                        // TODO IMPLEMENT JSON AS ARRAY
                    }
                    break;
                    
                    default: break;
                }
                
                ResetBuffer(TempBuffer, &TempBufferSize);
            }
        }
    }
    
    return ReadChars;
}