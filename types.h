#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef int32_t b32;


typedef float f32;
typedef double f64;

#define global static
#define local static
#define function_global static
#define internal_f static

#define ArrayCount(Array) sizeof(Array) / sizeof((Array)[0])
