#pragma once

#include <cmath>
#include <cstdint>

typedef size_t   usize;
typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
typedef float_t  f32;
typedef double_t f64;

typedef Vector2 v2;
typedef Vector3 v3;
typedef Vector2 p2;
typedef Vector3 p3;

// TODO Why is this needed?
#if defined(PLATFORM_WEB)
#define INT_MAX 0x7fffffff
#define INT_MIN (- 0x7fffffff) - 1
#endif