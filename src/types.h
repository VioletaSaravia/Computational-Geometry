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

v2 operator-(v2 const& lhs, v2 const& rhs) {
    return v2{lhs.x - rhs.x, lhs.y - rhs.y};
}

v2 operator+(v2 const& lhs, v2 const& rhs) {
    return v2{lhs.x + rhs.x, lhs.y + rhs.y};
}

v2 operator*(f32 const& lhs, v2 const& rhs) {
    return v2{lhs * rhs.x, lhs * rhs.y};
}

v2 operator/(v2 const& lhs, f32 const& rhs) {
    return v2{lhs.x / rhs, lhs.y / rhs};
}

v3 operator-(v3 const& lhs, v3 const& rhs) {
    return v3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

v3 operator+(v3 const& lhs, v3 const& rhs) {
    return v3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
}

v3 operator*(f32 const& lhs, v3 const& rhs) {
    return v3{lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
}

v3 operator/(v3 const& lhs, f32 const& rhs) {
    return v3{lhs.x / rhs, lhs.y / rhs, lhs.z / rhs};
}

// TODO Why is this needed?
#if defined(PLATFORM_WEB)
#define INT_MAX 0x7fffffff
#define INT_MIN (-0x7fffffff) - 1
#endif