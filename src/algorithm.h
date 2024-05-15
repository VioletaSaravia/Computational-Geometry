#pragma once

#include <cassert>

#include "types.h"

namespace math {

template <typename T>
T Min(const T& a, const T& b) {
    return a <= b ? a : b;
}

template <typename T>
T Max(const T& a, const T& b) {
    return a >= b ? a : b;
}

}  // namespace math

template <typename T>
class Array {
   public:
    T* buffer;

    usize size;
    usize count;

    Array(usize _size)
        : buffer(static_cast<T*>(malloc(sizeof(T) * _size))), size(_size), count(0){};

    Array(usize _size, T fill)
        : buffer(static_cast<T*>(malloc(sizeof(T) * _size))), size(_size), count(_size) {
        for (usize i = 0; i < count; i++) {
            buffer[i] = fill;
        }
    };

    void Push(const T& val) {
        assert(count + 1 <= size);

        buffer[count] = val;
        count++;
    }

    void Pop() {
        assert(count > 0);
        count--;
    }

    T& operator[](const usize idx) {
        assert(idx < count);
        return buffer[idx];
    }

    const T& operator[](const usize idx) const {
        assert(idx < count);
        return buffer[idx];
    }

    void Clear() { count = 0; }

    T Max() {
        assert(count > 0);

        T result = buffer[0];
        for (usize i = 0; i < count; i++) {
            if (buffer[i] > result)
                result = buffer[i];
        }

        return result;
    }

    T Min() {
        assert(count > 0);

        T result = buffer[0];
        for (usize i = 0; i < count; i++) {
            if (buffer[i] < result)
                result = buffer[i];
        }

        return result;
    }

    // O(n) maximum gap
    // Can be optimized by finding min and max while distributing in buckets
    // instead of using Min() and Max()
    // FIXME Rounding error somewhere?
    T MaxGap() {
        T max = Max();
        T min = Min();
        T gap = ceil((max - min) / (count - 1));

        Array<T> min_bucket(count - 1, INT_MAX);
        Array<T> max_bucket(count - 1, INT_MIN);

        for (usize i = 0; i < count; i++) {
            if (buffer[i] == min || buffer[i] == max)
                continue;

            T index           = (buffer[i] - min) / gap;
            min_bucket[index] = math::Min(min_bucket[index], buffer[i]);
            max_bucket[index] = math::Max(max_bucket[index], buffer[i]);
        }

        T max_gap  = 0;
        T prev_max = min;

        for (usize i = 0; i < count - 1; i++) {
            if (min_bucket[i] == INT_MAX && max_bucket[i] == INT_MIN) {
                continue;
            }
            max_gap  = math::Max(max_gap, min_bucket[i] - prev_max);
            prev_max = max_bucket[i];
        }

        return math::Max(max_gap, max - prev_max);
    }
};