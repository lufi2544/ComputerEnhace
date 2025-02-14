
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
            if(Value.Json)
            {
                delete Value.Json;                
            }
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



/*
  * Json Parser that is meant to have a Json file as input and then parse it to some object that we can extract values from.
*/

function_global enum_json_token GetToken(char Character)
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
    else if(Character == '-')
    {
        return enum_json_token::token_LetterOrNumber;
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

function_global bool CheckFlag(u16 Flags, enum_parser_flags FlagToCheck)
{
    return Flags & (1 << static_cast<u16>(FlagToCheck));
}

function_global void SetFlag(u16& Flags, enum_parser_flags FlagToSet, bool V)
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


function_global void ResetBuffer(char* Buffer, u32 *Size)
{
    memset(Buffer, '\0', *Size);
    *Size = 0;
}

function_global char* MakeBuffer(u32 Size)
{
    char* result = (char*)malloc(sizeof(char) * Size + 1);
    memset(result,'\0', Size + 1);
    
    return result;
}

function_global char* MakeBufferCopy(char *Buffer, u32 BufferSize)
{
    char* result = MakeBuffer(BufferSize);
    strncpy(result, Buffer, BufferSize);
    
    return result;
}

function_global json_category MakeJsonCategory(category_context *CategoryContext, json_value CategoryValue, enum_json_value_type Type)
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

function_global void PushChar(char V, char *Buffer, u32& Size)
{
    Buffer[Size++] = V;
}

function_global void EvaluateCategory(json_category *Category, char *TempBuffer, u32 *TempBufferSize)
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
        Category->Value.Number = strtod(TempBuffer, NULL);
    }
    
    ResetBuffer(TempBuffer, TempBufferSize);
}


function_global void EvaluateArrayValue(u16 Flags, enum_json_value_type *Type, char* TempBuffer, u32 TempBufferSize)
{
    if((strncmp(TempBuffer, "true", TempBufferSize) == 0) || (strncmp(TempBuffer, "false", TempBufferSize) == 0))
    {
        *Type = type_Bool;
    }
    else
    {
        // TODO EXCLUDE WHEN HAVING A JSON
        *Type = type_Number;
    }
    // String type is checked when we parse the double quote
}


////////////////////////////////////
////      JSON OBJECT          /////
////////////////////////////////////

// Maybe making a different struct for this object, so we can have a handler and a json 
// because in the subcategory is a bit weird.
struct json_object
{
    
    json_object() = default;
    json_object(const char* FileName)
    {
        PROFILE_FUNCTION();
        u32 BufferSize = 0;
        char* Buffer = nullptr;                
        FILE* file = nullptr;                        
        file =  fopen(FileName, "rb");
        
        if(!file)
        {
            fprintf(stderr, "Could not open the JSON file...");
            return;
        }
        
        fseek(file, 0, SEEK_END);
        BufferSize = ftell(file);
        rewind(file);
        Buffer = MakeBuffer(BufferSize);
        
        {
            PROFILE_BLOCK("Memory Allocation")
          // Get the file size;
          fread(Buffer, sizeof(char), BufferSize, file);
          fclose(file);
        }
        
        Name = FileName;        
        ParseBuffer(Buffer, BufferSize, 0);        
    }
    
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
    
    
    u32 ParseBuffer(const char* Buffer, u32 BufferSize, u32 FirstIndex = 0)
    {
        u32 ReadChars = 0;
        u16 Flags = 0;
        
        // TEMP DATA
        char TempBuffer[256];
        memset(TempBuffer, '\0', sizeof(TempBuffer));
        
        temp_array_data JsonArrayData;
        
        
        bool bTempDataFree = false;
        
        u8 InitialTempData = 100;
        u32 CurrentTempDataBufferSize = 0;
        
        
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
                    if(!CheckFlag(Flags, enum_parser_flags::flag_Array_Opened))
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
                        json_object* SubJson = new json_object();
                        //new (SubJson) json_object();
                        
                        
                        u32 SubJsonReadChars = SubJson->ParseBuffer(Buffer, BufferSize, Index + 1);
                        Index += SubJsonReadChars;
                        TempCategory.Value.Json = SubJson;
                        PushJsonCategory(&TempCategory);
                    }
                    else
                    {
                        
                        // Handle adding a json to an array of jsons.
                        if((JsonArrayData.Size + 1) > (JsonArrayData.ArrayJsonContext.Size))
                        {
                            if(JsonArrayData.ArrayJsonContext.Size == 0)
                            {
                                JsonArrayData.ArrayJsonContext.Array.JsonObjectArray = nullptr;
                            }
                            
                            JsonArrayData.ArrayJsonContext.Size += InitialTempData;
                            JsonArrayData.ArrayJsonContext.Array.JsonObjectArray = (json_object**)realloc(JsonArrayData.ArrayJsonContext.Array.JsonObjectArray, JsonArrayData.ArrayJsonContext.Size * sizeof(json_object*));
                        }
                        
                        JsonArrayData.Type = type_Json;
                        json_object* json = new json_object();
                        JsonArrayData.ArrayJsonContext.Array.JsonObjectArray[JsonArrayData.Size++] = json;
                        u32 SubJsonReadChars = json->ParseBuffer(Buffer, BufferSize, Index + 1);
                        Index += SubJsonReadChars;
                    }
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
                        else
                        {
                            JsonArrayData.Type = enum_json_value_type::type_String;
                        }
                    }
                    else
                    {
                        // Category Buffer
                        category_context CategoryContext;
                        CategoryContext.Key = TempBuffer;
                        CategoryContext.Size = TempBufferSize;
                        
                        TempCategory = MakeJsonCategory(&CategoryContext, json_value(), enum_json_value_type::type_None);
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
                if(TempCategory.ValueType == type_None && !CheckFlag(Flags, enum_parser_flags::flag_Array_Opened) && (!CheckFlag(Flags, enum_parser_flags::flag_String_Opened)))
                {
                    TempCategory.ValueType = enum_json_value_type::type_Number;
                }
                
            }
            else if((!CheckFlag(Flags, enum_parser_flags::flag_Array_Opened)) && (Token == enum_json_token::token_Coma || Token == enum_json_token::token_CloseBraces))
            {
                // CHANGING THE CATEGORY
                EvaluateCategory(&TempCategory, TempBuffer, &TempBufferSize);
                PushJsonCategory(&TempCategory);
                
                if (Token == token_CloseBraces)
                {
                    // Always break if we have the close braces
                    break;
                }
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
                PushArrayValue(&JsonArrayData, Flags, TempBuffer, &TempBufferSize);
                
                // PUSH THE ARRAY TO THE JSON
                PushArray(&TempCategory, &JsonArrayData);
            }
            else if(Token == enum_json_token::token_Coma)
            {
                // we are reading through the Array
                if(CheckFlag(Flags, enum_parser_flags::flag_Array_Opened))
                {
                    PushArrayValue(&JsonArrayData, Flags, TempBuffer, &TempBufferSize);
                }
            }
        }
        
        return ReadChars;
    }
    
    void PushJsonCategory(json_category *Category)
    {
        if(Category->ValueType != enum_json_value_type::type_None)
        {
            u32 TempSize = Size;
            Categories = (json_category*)realloc(Categories, ++TempSize * sizeof(json_category));
            Categories[Size++] = *Category;
            *Category = json_category();
        }
    }
    
    void Print()
    {       
        PROFILE_FUNCTION();
        if(Name != nullptr)
        {
            printf("Printing Json: %s \n", Name);
        }
        
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
                
                
                for(int ArrayIndex = 0; ArrayIndex < ArrayRef.Size; ArrayIndex++)
                {
                    switch (ArrayRef.Type)
                    {
                        case type_Number:
                        {
                            // TODO TYPE CHECK HERE
                            float Num = ArrayRef.Value.NumberArray[ArrayIndex];
                            
                            if(ArrayIndex < ArrayRef.Size - 1)
                            {
                                printf("%.8f , ", Num);
                            }
                            else
                            {
                                printf("%.8f", Num);
                            }
                            
                        }
                        break;
                        
                        case type_String:
                        {
                            
                            char* String = ArrayRef.Value.StringArray[ArrayIndex];
                            if(ArrayIndex < ArrayRef.Size - 1)
                            {
                                printf("%s ,", String);
                            }
                            else
                            {
                                printf("%s", String);
                            }
                        }
                        break;
                        
                        case type_Bool:
                        {
                            
                            bool Value = ArrayRef.Value.BoolArray[ArrayIndex];
                            const char* BoolValue = (Value == true) ? "true" : "false";
                            if (ArrayIndex == ArrayRef.Size - 1)
                            {
                                printf("%s", BoolValue);
                            }
                            else
                            {
                                printf("%s, ", BoolValue);
                            }
                            
                        }
                        break;
                        
                        case type_Json:
                        {
                            printf("{ \n");
                            ArrayRef.Value.JsonObjectArray[ArrayIndex]->Print();
                            if(ArrayIndex == ArrayRef.Size - 1)
                            {
                                printf("} \n");
                            }
                            else
                            {
                                printf("}, \n");
                            }
                            
                        }
                        break;
                        
                        default:
                        break;
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
                    snprintf(Buffer, sizeof(Buffer), "%.8f", V.Number);
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
    
    
    private:
    void PushArrayValue(temp_array_data* ArrayData, u16 Flags, char* TempBuffer, u32* TempBufferSize)
    {
        u8 InitialTempData = 100;
        if (ArrayData->Type == type_None)
        {
            EvaluateArrayValue(Flags, &ArrayData->Type, TempBuffer, *TempBufferSize);
        }
        
        switch(ArrayData->Type)
        {
            case enum_json_value_type::type_Number:
            {
                f32* Array =ArrayData->ArrayNumberContxt.Array.NumberArray;
                u32& ArraySize = ArrayData->ArrayNumberContxt.Size;
                if((ArrayData->Size + 1) > (ArraySize))
                {
                    if (ArrayData->ArrayNumberContxt.Size == 0)
                    {
                        ArrayData->ArrayNumberContxt.Array.NumberArray = nullptr;
                    }
                    
                    ArraySize += InitialTempData;
                    Array = (f32*)realloc(Array, (ArraySize) * sizeof(f32));
                    ArrayData->ArrayNumberContxt.Array.NumberArray = Array;
                }
                
                Array[ArrayData->Size++] = atoll(TempBuffer);
            }
            break;
            
            case type_Bool:
            {
                
                if((ArrayData->Size + 1) > (ArrayData->ArrayBoolContext.Size))
                {
                    if(ArrayData->ArrayBoolContext.Size == 0)
                    {
                        ArrayData->ArrayBoolContext.Array.BoolArray = nullptr;
                    }
                    
                    ArrayData->ArrayBoolContext.Size += InitialTempData;
                    ArrayData->ArrayBoolContext.Array.BoolArray = (bool*)realloc(ArrayData->ArrayBoolContext.Array.BoolArray, (ArrayData->ArrayBoolContext.Size) * sizeof(bool));
                }
                
                bool* BoolArray =ArrayData->ArrayBoolContext.Array.BoolArray;
                bool bValueToAdd = !(strncmp(TempBuffer, "false", *TempBufferSize) == 0);
                BoolArray[ArrayData->Size++] = bValueToAdd;
            }
            break;
            
            case type_String:
            {
                
                if((ArrayData->Size + 1) > (ArrayData->ArrayStringContext.Size))
                {
                    if (ArrayData->ArrayStringContext.Size == 0)
                    {
                        ArrayData->ArrayStringContext.Array.StringArray = nullptr;
                    }
                    
                    ArrayData->ArrayStringContext.Size += InitialTempData;
                    ArrayData->ArrayStringContext.Array.StringArray  = (char**)realloc(ArrayData->ArrayStringContext.Array.StringArray, (ArrayData->ArrayStringContext.Size) * sizeof(char*));
                }
                
                char** Array = ArrayData->ArrayStringContext.Array.StringArray;
                
                char* String = (char*)malloc(sizeof(char) * (*TempBufferSize) + 1);
                memset(String, '\0', *TempBufferSize + 1);
                memcpy(String, TempBuffer, *TempBufferSize);
                Array[ArrayData->Size++] = String;
            }
            break;
            
            case type_Json:
            {
                
            }
            break;
            
            default: break;
        }
        
        ResetBuffer(TempBuffer, TempBufferSize);
    }
    
    void PushArray(json_category* TempCategory, temp_array_data* ArrayData)
    {
        json_array& ArrayRef = TempCategory->Value.JsonArray;
        ArrayRef.Size = ArrayData->Size;
        ArrayRef.Type = ArrayData->Type;
        
        switch(ArrayRef.Type)
        {
            case enum_json_value_type::type_Number:
            {
                u32 Bytes = sizeof(f32) * ArrayRef.Size;
                f32* AsNumbers = (f32*)malloc(Bytes);
                memcpy(AsNumbers, ArrayData->ArrayNumberContxt.Array.NumberArray, Bytes);
                ArrayRef.Value.NumberArray = AsNumbers;
            }
            break;
            
            case enum_json_value_type::type_Bool:
            {
                u32 Bytes = sizeof(bool) * ArrayRef.Size;
                bool* AsBools = (bool*)malloc(Bytes);
                memcpy(AsBools, ArrayData->ArrayBoolContext.Array.BoolArray, Bytes);
                ArrayRef.Value.BoolArray = AsBools;
            }
            break;
            
            case enum_json_value_type::type_String:
            {
                u32 Bytes = sizeof(char*) * ArrayRef.Size;
                char** AsString = (char**)malloc(Bytes);
                
                for(int i = 0; i < ArrayRef.Size; ++i)
                {
                    char* String = ArrayData->ArrayStringContext.Array.StringArray[i];
                    u32 StringToCpySize = strlen(String) + 1;
                    char* DestString = (char*)malloc(sizeof(char)* StringToCpySize);
                    memcpy(DestString, String, StringToCpySize);
                    
                    AsString[i] = DestString;
                }
                
                memcpy(AsString, ArrayData->ArrayStringContext.Array.StringArray, Bytes);
                ArrayRef.Value.StringArray = AsString;
            }
            break;
            
            case enum_json_value_type::type_Json:
            {
                ArrayRef.Value.JsonObjectArray = ArrayData->ArrayJsonContext.Array.JsonObjectArray;
                ArrayData->ArrayJsonContext.Array.JsonObjectArray = nullptr;
            }
            break;
            
            default: break;
        }
        
        
        // Reseting the ArraySize to reuse it.
        ArrayData->Size = 0;
    }
    
    public:
    const char* Name = nullptr;
    json_category* Categories = nullptr;
    s32 Size = 0;
};