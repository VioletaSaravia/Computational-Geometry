#pragma once

#include <cassert>
#include <climits>

#include "types.h"

enum ArenaType { Cycle, Throw };

class Arena {
    u8* buffer;

   public:
    usize     size;
    usize     count = 0;
    ArenaType type  = Cycle;

    explicit Arena(usize _size) : buffer(new u8[_size]), size(_size){};
    Arena(const Arena&)            = delete;
    Arena& operator=(const Arena&) = delete;
    // Multiple deletes if multiple objects are using/holding the same arena
    // ~Arena() { delete[] buffer; }

    template <typename T>
    T* Alloc(usize count = 1) {
        this->count += sizeof(T) * count;
        if (this->count >= size)
            switch (type) {
                case Cycle:
                    Clear();
                    break;

                case Throw:
                    throw "Arena overflow";

                    // default:
                    //     std::unreachable();
            }

        return reinterpret_cast<T*>(&buffer[this->count]);
    }

    inline void Clear() { count = 0; }
};

// TODO
class PackedStringArray {};

template <typename T>
class Array {
    Arena* arena;

   public:
    T*    buffer;  // FIXME Left public for std::sort?
    usize size;
    usize count;

    explicit Array(usize _size, Arena* _arena = nullptr)
        : arena(_arena ? _arena : new Arena(_size * sizeof(T))),
          buffer(arena->Alloc<T>(_size)),
          size(_size),
          count(0){};

    explicit Array(usize _size, T fill, Arena* _arena = nullptr)
        : arena(_arena ? _arena : new Arena(_size * sizeof(T))),
          buffer(arena->Alloc<T>(_size)),
          size(_size),
          count(_size) {
        for (usize i = 0; i < count; i++) {
            buffer[i] = fill;
        }
    };

    Array<T>& operator=(const Array<T>&) = default;
    Array<T>& operator=(Array<T>&)       = default;

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

    T Max() const {
        assert(count > 0);

        T result = buffer[0];
        for (usize i = 0; i < count; i++) {
            if (buffer[i] > result)
                result = buffer[i];
        }

        return result;
    }

    T Min() const {
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

        // TODO Arena allocate
        Array<T> min_bucket(count - 1, INT_MAX, &arena);
        Array<T> max_bucket(count - 1, INT_MIN, &arena);

        for (usize i = 0; i < count; i++) {
            if (buffer[i] == min || buffer[i] == max)
                continue;

            T index           = (buffer[i] - min) / gap;
            min_bucket[index] = std::min(min_bucket[index], buffer[i]);
            max_bucket[index] = std::max(max_bucket[index], buffer[i]);
        }

        T max_gap  = 0;
        T prev_max = min;

        for (usize i = 0; i < count - 1; i++) {
            if (min_bucket[i] == INT_MAX && max_bucket[i] == INT_MIN) {
                continue;
            }
            max_gap  = std::max(max_gap, min_bucket[i] - prev_max);
            prev_max = max_bucket[i];
        }

        return std::max(max_gap, max - prev_max);
    }
};
