
#include "json_parser.h"
#define _CRT_SECURE_NO_WARNINGS

int main(int ArgsCount,char** Args)
{
    json_object json("input.json");
    json.Print();
    return 0;
}