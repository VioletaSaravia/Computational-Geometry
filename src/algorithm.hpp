#pragma once

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

namespace fs = std::filesystem;
namespace ch = std::chrono;

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

struct Vector2i {
    i32 x, y;
};

struct Vector2u {
    u32 x, y;
};

typedef Vector2i v2i;
typedef Vector2i p2i;

typedef Vector2u v2u;
typedef Vector2u p2u;

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

enum ArenaType { Cycle, Throw };

class ArenaOverflow : public std::exception {
   public:
    const char* what() { return "Arena allocation overflowed"; }
};

class Arena {
    u8* buffer;

   public:
    usize     size;
    usize     count = 0;
    ArenaType type  = Cycle;

    explicit Arena(usize _size) : buffer(new u8[_size]), size(_size) {};
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
                    throw ArenaOverflow();

                    // default:
                    //     std::unreachable();
            }

        return reinterpret_cast<T*>(&buffer[this->count]);
    }

    inline void Clear() { count = 0; }
};

// TODO
class PackedStringArray {};

#define DEFAULT_ARENA_SIZE 16384  // 16Kb

template <typename T>
class Array {
    Arena* arena;

   public:
    T*    buffer;  // FIXME Left public for std::sort?
    usize size;
    usize count;
    usize stride = 1;

    explicit Array(std::initializer_list<T> list, Arena* _arena = nullptr)
        : arena(_arena ? _arena : new Arena(list.size() * sizeof(T))),
          buffer(arena->Alloc<T>(list.size())),
          size(list.size()),
          count(0) {
        for (auto&& elem : list) Push(elem);
    }

    explicit Array(usize _size, Arena* _arena = nullptr)
        : arena(_arena ? _arena : new Arena(_size * sizeof(T))),
          buffer(arena->Alloc<T>(_size)),
          size(_size),
          count(0) {};

    explicit Array(usize _size, const T& fill, Arena* _arena = nullptr)
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

    T& operator[](const v2i coords) {
        assert(coords.x + coords.y * stride < count);
        return buffer[(usize)(coords.x + coords.y * stride)];
    }

    const T& operator[](const v2i coords) const {
        assert(coords.x + coords.y * stride < count);
        return buffer[(usize)(coords.x + coords.y * stride)];
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

template <typename T, u8 D>
class ArrayDim : public Array<T> {
   public:
    u8 strides[D];
};

// Types that satisfy Dampenable may be used as type parameters for Damped<> without causing
// errors, though it may not make sense to do so (e.g. Damped<bool> satisfies this constraint, even
// though a damped bool is rather meaningless).
template <typename T>
concept Dampenable = requires(T const& value, f32 const& scalar) {
    { value + value } -> std::convertible_to<T>;
    { value - value } -> std::convertible_to<T>;
    { scalar* value } -> std::convertible_to<T>;
    { value / scalar } -> std::convertible_to<T>;
};

// TODO A pool of damped values?
template <Dampenable T>
class Damped {
    bool enabled = true;
    bool started = false;

    T   target{};
    T   xp, yd, y;
    f32 k1, k2, k3;
    f32 f, z, r;

    // Sets the starting point of the damping function. Also allows Damped to not fail when starting
    // at frame zero, i.e. when frame delta time is zero.
    T start(T x0) {
        y      = x0;
        yd     = x0;
        xp     = x0;
        target = x0;

        started = true;
        return y;
    }

   public:
    T value() { return y; }

    explicit Damped(const f32 _f = 1, const f32 _z = 1, const f32 _r = 0)
        : k1(_z / (PI * _f)),
          k2(1 / powf(2 * PI * _f, 2)),
          k3(_r * _z / (2 * PI * _f)),
          f(_f),
          z(_z),
          r(_r) {}

    // TODO Better interface. Maybe v3{} for parameters.
    explicit Damped(
        const T _target, bool _started, const f32 _f = 1, const f32 _z = 1, const f32 _r = 0)
        : started(_started),
          target(_target),
          k1(_z / (PI * _f)),
          k2(1 / powf(2 * PI * _f, 2)),
          k3(_r * _z / (2 * PI * _f)),
          f(_f),
          z(_z),
          r(_r) {}

    // When toggled off, target is passed through with no damping. On by default.
    void Toggle() { enabled = !enabled; }

    // Computes a new damped value tending towards the previous target + offset.
    T By(const T offset) {
        target = target + offset;
        return Set(target);
    }

    // Computes a new damped value tending towards the existing target.
    T Set() { return Set(target); }

    // Computes a new damped value tending towards a new target.
    T Set(const T newTarget) {
        if (!started)
            return start(newTarget);

        if (!enabled) {
            y = newTarget;
            return y;
        }

        f32 delta = GetFrameTime();

        T xd   = (newTarget - xp) / delta;
        xp     = newTarget;
        target = newTarget;
        y      = y + delta * yd;

        f32 k2_stable = std::max(k2, f32(1.1 * (delta * delta / 4 + delta * k1 / 2)));
        yd            = yd + delta * (newTarget + k3 * xd - y - k1 * yd) / k2_stable;
        return y;
    }
};

enum CellState {
    Dead,
    Alive,
};

// TODO Cellular Automata GDExtension plugin
// custom number of states, history, rules, etc etc.
// Inherits from tilemap?
struct ConwayBoard {
    // TODO two copies of board for data-oriented?
    // And if I add a time dimension to determine the tile to use? What's more data-oriented then?
    CellState board[64][64];

    ConwayBoard(Array<v2> initialLiving) /*/: board{}*/ {
        memset(board, CellState::Dead, sizeof(CellState) * 64 * 64);

        for (usize i = 0; i < initialLiving.count; i++)
            board[(usize)initialLiving[i].x][(usize)initialLiving[i].y] = CellState::Alive;
    }

    void Update() {
        for (usize i = 1; i < 63; i++)
            for (usize j = 1; j < 63; j++) {
                usize near{};

                for (usize k = i - 1; k < i + 2; k++)
                    for (usize f = j - 1; f < j + 2; f++)
                        near += board[k][f] == CellState::Alive ? 1 : 0;

                // RULES HERE
                switch (near) {
                    case 0:
                        break;

                    case 1:
                        break;

                    case 2:
                        break;

                    case 3:
                        break;

                    default:
                        break;
                }
            }
    }

    void Draw() {}
};

struct Edge {
    v2 p, q;

    f32  AngleTo(Edge l) const;
    bool Intersects(Edge l) const;
};

namespace vec2 {

f32 DistanceTo(const v2& p, const v2& q) {
    return std::sqrt(std::pow((q.x - p.x), 2) + std::pow((q.y - p.y), 2));
}

bool IsLeft(const v2 p, const v2 q) {
    return (p.x - q.x) * (p.y - q.y) - (p.y - q.y) * (p.x - q.x) > 0;
}

bool IsLeft(const v2 p, const Edge l) {
    return (l.p.x - l.q.x) * (p.y - l.q.y) - (l.p.y - l.q.y) * (p.x - l.q.x) > 0;
}

bool IsLeft(const v2 p, const v2& a, const v2& b) {
    return (a.x - b.x) * (p.y - b.y) - (a.y - b.y) * (p.x - b.x) > 0;
}

bool IsInTriangle(const v2 p, const v2 q, const v2 r) {
    return IsLeft(p, q) && IsLeft(q, r) && IsLeft(r, p);
}

bool IsInRectangle(const v2 p, const Rectangle rect) {
    return IsLeft(p, v2{rect.x + rect.width, rect.y}, v2{rect.x, rect.y}) &&
           IsLeft(
               p, v2{rect.x + rect.width, rect.y + rect.height}, v2{rect.x + rect.width, rect.y}) &&
           IsLeft(p, v2{rect.x, rect.y}, v2{rect.x, rect.y + rect.height}) &&
           IsLeft(
               p, v2{rect.x, rect.y + rect.height}, v2{rect.x + rect.width, rect.y + rect.height});
}

}  // namespace vec2

namespace rect {

bool Intersects(const Rectangle& p, const Rectangle& q) {
    return p.x < q.x + q.width && p.x + p.width > q.x && p.y < p.y + q.height &&
           p.y + p.height > q.y;
}

i32 RectangleOverlapSize(const Rectangle& p, const Rectangle& q) {
    if (!Intersects(p, q))
        return 0;

    return std::min(p.x + p.width, q.x + q.width) -
           std::max(p.x, q.x) * std::min(p.y + p.height, q.y + q.height) - std::max(p.y, q.y);
}

Rectangle RectangleOverlap(const Rectangle& p, const Rectangle& q) {
    if (!Intersects(p, q))
        return Rectangle{};

    return Rectangle{std::max(q.x, p.x),
                     std::max(q.y, p.y),
                     std::min(p.x + p.width, q.x + q.width) - std::max(p.x, q.x),
                     std::min(p.y + p.height, q.y + q.height) - std::max(p.y, q.y)};
}

}  // namespace rect

namespace circle2 {

bool Intersects(const v2& p_pos, const u32 p_radius, const v2& q_pos, const u32 q_radius) {
    return vec2::DistanceTo(p_pos, q_pos) < p_radius + q_radius;
}

}  // namespace circle2

f32 Edge::AngleTo(const Edge l) const {
    return atan2(l.q.y - l.p.y, l.q.x - l.p.x) - atan2(q.y - p.y, q.x - p.x);
}

bool Edge::Intersects(const Edge l) const {
    return vec2::IsLeft(l.p, *this) != vec2::IsLeft(l.q, *this) &&
           vec2::IsLeft(p, l) != vec2::IsLeft(q, l);
}

namespace mesh {

v2 Centroid(const Array<v2>& mesh) {
    v2 result{};

    for (usize i = 0; i < mesh.count; i++) {
        result.x += mesh[i].x;
        result.y += mesh[i].y;
    }

    result.x /= mesh.count;
    result.y /= mesh.count;

    return result;
}

v2 Centroid(const Array<Edge>& mesh) {
    v2 result{};

    for (usize i = 0; i < mesh.count - 1; i++) {
        result.x += mesh[i].p.x;
        result.y += mesh[i].p.x;
    }

    result.x /= mesh.count;
    result.y /= mesh.count;

    return result;
}

}  // namespace mesh

struct Circle {
    f32 x;
    f32 y;
    f32 r;
    Circle() {}
    Circle(f32 x, f32 y, f32 r) : x(x), y(y), r(r) {}
    Circle(v2 xy, f32 r) : x(xy.x), y(xy.y), r(r) {}
};

Circle _EnclosingDisk(Array<v2> const& R) {
    switch (R.count) {
        case 0:
            return Circle(0, 0, -1);

        case 1:
            return Circle(R[0].x, R[0].y, 0);

        case 2:
            return Circle((R[0].x + R[1].x) / 2.0,
                          (R[0].y + R[1].y) / 2.0,
                          hypot(R[0].x - R[1].x, R[0].y - R[1].y) / 2.0);

        default:
            f32 D = (R[0].x - R[2].x) * (R[1].y - R[2].y) - (R[1].x - R[2].x) * (R[0].y - R[2].y);
            f32 p0 =
                (((R[0].x - R[2].x) * (R[0].x + R[2].x) + (R[0].y - R[2].y) * (R[0].y + R[2].y)) /
                     2 * (R[1].y - R[2].y) -
                 ((R[1].x - R[2].x) * (R[1].x + R[2].x) + (R[1].y - R[2].y) * (R[1].y + R[2].y)) /
                     2 * (R[0].y - R[2].y)) /
                D;
            f32 p1 =
                (((R[1].x - R[2].x) * (R[1].x + R[2].x) + (R[1].y - R[2].y) * (R[1].y + R[2].y)) /
                     2 * (R[0].x - R[2].x) -
                 ((R[0].x - R[2].x) * (R[0].x + R[2].x) + (R[0].y - R[2].y) * (R[0].y + R[2].y)) /
                     2 * (R[1].x - R[2].x)) /
                D;
            return Circle(p0, p1, hypot(R[0].x - p0, R[0].y - p1));
    }
}

Circle _EnclosingDisk(Array<v2>& P, usize i, Array<v2> R) {
    if (i == P.count || R.count == 3)
        return _EnclosingDisk(R);

    Circle D = _EnclosingDisk(P, i + 1, R);
    if (hypot(P[i].x - D.x, P[i].y - D.y) > D.r) {
        R.Push(P[i]);
        D = _EnclosingDisk(P, i + 1, R);
    }
    return D;
}

// Welzl's smallest enclosing disk algorithm
// fails if two points are in the same place
// points should ideally be randomly shuffled.
Circle EnclosingDisk(Array<v2> P) {
    static Arena EnclosingDiskArena(DEFAULT_ARENA_SIZE);
    return _EnclosingDisk(P, 0, Array<v2>(P.size, &EnclosingDiskArena));
}

Array<Edge> ConvexHull_ExtremeEdges(const Array<v2>& points) {
    usize       size = 40;
    Array<Edge> result(size);
    usize       count = 0;

    for (usize i = 0; i < points.count; ++i) {
        for (usize j = 0; j < points.count; ++j) {
            if (memcmp(&points[i], &points[j], sizeof(v2)) == 0)
                continue;

            const Edge toTest    = Edge{points[i], points[j]};
            bool       isExtreme = true;

            for (usize k = 0; k < points.count; ++k) {
                if (memcmp(&points[k], &toTest.p, sizeof(v2)) == 0)
                    continue;
                if (memcmp(&points[k], &toTest.q, sizeof(v2)) == 0)
                    continue;

                // TODO: tmb chequear si cloud[k] esta en la linea pq

                if (vec2::IsLeft(points[k], toTest)) {
                    isExtreme = false;
                    break;
                }
            }

            if (isExtreme) {
                result.Push(toTest);

                // if ring is closed
                if (memcmp(&result[count].q, &result[0].p, sizeof(v2)) == 0)
                    return result;
                // break?
            }
        }
    }

    return result;
};

Array<Edge> ConvexHull_JarvisMarch(const Array<v2>& points) {
    usize       size = 40;  // TODO
    Array<Edge> result(size);

    // TODO: Degenerate case (multiple mins)
    std::sort(&points.buffer[0], &points.buffer[points.count], [](const v2 a, const v2 b) {
        return a.x > b.x;
    });

    usize i = 0;
    for (usize j = 0; j < points.count; ++j) {
        if (memcmp(&points[i], &points[j], sizeof(v2)) == 0)
            continue;

        // TODO: optimizar copy
        const Edge toTest    = Edge{points[i], points[j]};
        bool       isExtreme = true;

        for (usize k = 0; k < points.count; ++k) {
            if (memcmp(&points[k], &toTest.p, sizeof(v2)) == 0)
                continue;
            if (memcmp(&points[k], &toTest.q, sizeof(v2)) == 0)
                continue;

            // TODO: tmb chequear si cloud[k] esta en la linea pq

            if (vec2::IsLeft(points[k], toTest)) {
                isExtreme = false;
                break;
            }
        }

        if (isExtreme) {
            result.Push(toTest);
            i = j;
            break;
        }
    }

    do {
        f64   smallest_angle = INT_MAX;
        usize smallest_j     = 0;
        for (usize j = 0; j < points.count; ++j) {
            if (memcmp(&points[i], &points[j], sizeof(v2)) == 0)
                continue;

            // TODO: optimizar copy
            const Edge toTest = Edge{points[i], points[j]};

            f64 angle = result[result.count - 1].AngleTo(toTest);
            if (angle < 0)
                angle += 2 * PI;  // ???

            // TODO: Degenerate case (point in same line as ij)
            if (angle < smallest_angle) {
                smallest_angle = angle;
                smallest_j     = j;
            }
        }

        result.Push(Edge{result[result.count - 1].q, points[smallest_j]});
        i = smallest_j;
    } while (memcmp(&points[0], &result[result.count - 1].q, sizeof(v2)) != 0);

    return result;
}

// TODO mesh object
// sorted array for points
// O(n) graham scan
// updatable convex hull / bounding box / bounding circle
static Arena convexHullArena(DEFAULT_ARENA_SIZE);
Array<Edge>  ConvexHull_GrahamScan(const Array<v2>& points) {
    // Array<v2> points = basePoints;

    v2 first{FLT_MAX, 0};
    for (usize i = 0; i < points.count; ++i) {
        if (points[i].x < first.x) {
            first = points[i];
        }
    }

    // Ordenar puntos por angulo polar respecto de first
    // TODO mejor ordenar por X y comparar con un punto al infinito?
    Edge horizontal = Edge{first, v2{first.x + 1, first.y}};
    std::sort(&points.buffer[0],
              &points.buffer[points.count],
              [first, horizontal](const v2& a, const v2& b) {
                  if (memcmp(&a, &first, sizeof(v2)) == 0)
                      return true;
                  if (memcmp(&b, &first, sizeof(v2)) == 0)
                      return false;

                  return horizontal.AngleTo(Edge{first, a}) > horizontal.AngleTo(Edge{first, b});
              });

    Array<v2> resPoints(1 + points.count, &convexHullArena);

    resPoints.Push(points[0]);
    resPoints.Push(points[1]);

    // Graham scan main loop
    usize i = 2;
    while (i < points.count) {
        if (vec2::IsLeft(
                points[i], resPoints[resPoints.count - 2], resPoints[resPoints.count - 1])) {
            resPoints.Push(points[i]);
            i++;
        } else {
            resPoints.Pop();
        }
    }

    resPoints.Push(points[0]);

    // TODO Meh. Can be removed.
    Array<Edge> result(resPoints.count, &convexHullArena);

    for (usize j = 0; j < resPoints.count - 1; ++j) {
        result.Push(Edge{resPoints[j], resPoints[j + 1]});
    }

    return result;
};

template <typename T>
struct FileWatcher {
    const std::string basePath{};

    std::vector<T>                   loadedData;
    std::vector<fs::directory_entry> filePaths;

    std::vector<fs::file_time_type>              lastModified;
    std::function<T(fs::directory_entry const&)> loader;
    std::function<void(T&)>                      unloader;

    FileWatcher(std::string                                  _basePath,
                std::function<T(fs::directory_entry const&)> _loader,
                std::function<void(T&)>                      _unloader)
        : basePath{_basePath}, loader{_loader}, unloader{_unloader} {
        usize i = 0;
        for (auto const& file : fs::directory_iterator(fs::absolute(basePath))) {
            filePaths.push_back(file);
            loadedData.push_back(loader(file));
            lastModified.push_back(file.last_write_time());
            i++;
        }
    }

    void Update() {
        usize i = 0;
        for (auto const& file : fs::directory_iterator(fs::absolute(basePath))) {
            auto currentModified = file.last_write_time();
            if (currentModified != lastModified[i]) {
                printf_s("INFO: Reloading modified file: %s", file.path().c_str());

                unloader(loadedData[i]);
                loadedData[i]   = loader(file);
                lastModified[i] = currentModified;
            }
            i++;
        }
    }
};

typedef Rectangle Shape2D;  // TODO

template <typename T>
struct Collision {
    bool hit;
    v2   point;
    T    shape;
};

template <template <typename> class C, class T>
Collision<T> CastRay(v2 const& from, v2 const& to, C<T> const& colliders, f32 resolution = .01) {
    for (f32 t = 0; t < 1; t += resolution) {
        for (usize j = 0; j < colliders.count; j++) {
            v2 maybeHit = from + t * (to - from);  // TODO lerp
            if (vec2::IsInRectangle(maybeHit, colliders[j])) {
                return Collision{.hit = true, .point = maybeHit, .shape = colliders[j]};
            }
        }
    }

    return Collision{.hit = false, .point = v2{}, .shape = T{}};
}

template <typename T>
void SaveAsCsv(const char*                           filename,
               std::initializer_list<const Array<T>> data,
               const char*                           columns = NULL) {
    std::ofstream outFile(std::format("../{}", filename));
    if (!outFile) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    if (columns != NULL) {
        outFile << columns << ",\n";
    } else {
        for (usize i = 0; i < data.size(); i++) {
            outFile << "Value" << i << ",";
        }
        outFile << "\n";
    }

    Array<usize> sizes(data.size());
    for (auto& array : data) {
        sizes.Push(array.count);
    }

    for (usize i = 0; i < sizes.Min(); i++) {
        for (auto& array : data) {
            outFile << array[i];
            outFile << ", ";
        }
        outFile << "\n";
    }
    outFile.close();
}
