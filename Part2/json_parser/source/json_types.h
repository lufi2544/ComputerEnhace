/* date = December 20th 2024 9:40 am */

#ifndef JSON_TYPES_H
#define JSON_TYPES_H

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
};


enum enum_json_value_type
{
    type_None,
    type_Number,
    type_Bool,
    type_String,
    type_Json,
};

union json_array_value
{
    char** StringArray;
    float* NumberArray;
    bool* BoolArray;
    struct json_object* JsonObjectArray;
};

struct json_array
{
    json_array_value Value;
    enum_json_value_type Type;
    u8 Size;
};

union json_value
{
    char* String;
    float Number;
    bool Bool;
    struct json_object* Json;
    json_array Array;
};


#endif //JSON_TYPES_H
