#pragma once
#include <assert.h>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstring>

#include "algorithm.h"
#include "types.h"

struct Edge {
    v2 p, q;

    f32 AngleTo(Edge l) const;
    bool   Intersects(Edge l) const;
};

namespace vec2 {

f32 DistanceTo(const v2 &p, const v2 &q) {
    return std::sqrt(std::pow((q.x - p.x), 2) + std::pow((q.y - p.y), 2));
}

bool IsLeft(const v2 p, const v2 q) {
    return (p.x - q.x) * (p.y - q.y) - (p.y - q.y) * (p.x - q.x) > 0;
}

bool IsLeft(const v2 p, const Edge l) {
    return (l.p.x - l.q.x) * (p.y - l.q.y) - (l.p.y - l.q.y) * (p.x - l.q.x) > 0;
}

bool IsLeft(const v2 p, const v2 &a, const v2 &b) {
    return (a.x - b.x) * (p.y - b.y) - (a.y - b.y) * (p.x - b.x) > 0;
}

bool IsInTriangle(const v2 p, const v2 q, const v2 r) {
    return IsLeft(p, q) && IsLeft(q, r) && IsLeft(r, p);
}

}  // namespace vec2

namespace rect {

bool Intersects(const Rectangle &p, const Rectangle &q) {
    return p.x < q.x + q.width && p.x + p.width > q.x && p.y < p.y + q.height &&
           p.y + p.height > q.y;
}

i32 RectangleOverlapSize(const Rectangle &p, const Rectangle &q) {
    if (!Intersects(p, q))
        return 0;

    return std::min(p.x + p.width, q.x + q.width) -
           std::max(p.x, q.x) * std::min(p.y + p.height, q.y + q.height) - std::max(p.y, q.y);
}

Rectangle RectangleOverlap(const Rectangle &p, const Rectangle &q) {
    if (!Intersects(p, q))
        return Rectangle{};

    return Rectangle{std::max(q.x, p.x),
                     std::max(q.y, p.y),
                     std::min(p.x + p.width, q.x + q.width) - std::max(p.x, q.x),
                     std::min(p.y + p.height, q.y + q.height) - std::max(p.y, q.y)};
}

}  // namespace rect

namespace circle2 {

bool Intersects(const v2 &p_pos, const u32 p_radius, const v2 &q_pos, const u32 q_radius) {
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

v2 Centroid(const Array<v2> &mesh) {
    v2 result{};

    for (usize i = 0; i < mesh.count; i++) {
        result.x += mesh[i].x;
        result.y += mesh[i].y;
    }

    result.x /= mesh.count;
    result.y /= mesh.count;

    return result;
}

v2 Centroid(const Array<Edge> &mesh) {
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

Circle _EnclosingDisk(Array<v2> &R) {
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

Circle _EnclosingDisk(Array<v2> &P, usize i, Array<v2> R) {
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
Circle EnclosingDisk(Array<v2> P) {
    // std::random_shuffle(P.begin(), P.end());
    return _EnclosingDisk(P, 0, Array<v2>(P.size));
}

Array<Edge> ConvexHull_ExtremeEdges(const Array<v2> &points) {
    usize       size = 40;
    Array<Edge> result(size);
    usize       count = 0;

    for (usize i = 0; i < points.count; ++i) {
        for (usize j = 0; j < points.count; ++j) {
            if (memcmp(&points[i], &points[j], sizeof(v2)) == 0)
                continue;

            const Edge to_test    = Edge{points[i], points[j]};
            bool       is_extreme = true;

            for (usize k = 0; k < points.count; ++k) {
                if (memcmp(&points[k], &to_test.p, sizeof(v2)) == 0)
                    continue;
                if (memcmp(&points[k], &to_test.q, sizeof(v2)) == 0)
                    continue;

                // TODO: tmb chequear si cloud[k] esta en la linea pq

                if (vec2::IsLeft(points[k], to_test)) {
                    is_extreme = false;
                    break;
                }
            }

            if (is_extreme) {
                result.Push(to_test);

                // if ring is closed
                if (memcmp(&result[count].q, &result[0].p, sizeof(v2)) == 0)
                    return result;
                // break?
            }
        }
    }

    return result;
};

Array<Edge> ConvexHull_JarvisMarch(const Array<v2> &points) {
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
        const Edge to_test    = Edge{points[i], points[j]};
        bool       is_extreme = true;

        for (usize k = 0; k < points.count; ++k) {
            if (memcmp(&points[k], &to_test.p, sizeof(v2)) == 0)
                continue;
            if (memcmp(&points[k], &to_test.q, sizeof(v2)) == 0)
                continue;

            // TODO: tmb chequear si cloud[k] esta en la linea pq

            if (vec2::IsLeft(points[k], to_test)) {
                is_extreme = false;
                break;
            }
        }

        if (is_extreme) {
            result.Push(to_test);
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
            const Edge to_test = Edge{points[i], points[j]};

            f64 angle = result[result.count - 1].AngleTo(to_test);
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
// TODO this needs profiling. Let's take Casey Muratori's class!
Array<Edge> ConvexHull_GrahamScan(const Array<v2> &points) {
    v2 first{FLT_MAX, 0};
    for (usize i = 0; i < points.count; ++i) {
        if (points[i].x < first.x)
            first = points[i];
    }

    // Ordenar puntos por angulo polar respecto de first
    // TODO mejor ordenar por X y comparar con un punto al infinito?
    Edge horizontal = Edge{first, v2{first.x + 1, first.y}};
    std::sort(&points.buffer[0],
              &points.buffer[points.count],
              [first, horizontal](const v2 &a, const v2 &b) {
                  if (memcmp(&a, &first, sizeof(v2)) == 0)
                      return true;
                  if (memcmp(&b, &first, sizeof(v2)) == 0)
                      return false;

                  return horizontal.AngleTo(Edge{first, a}) > horizontal.AngleTo(Edge{first, b});
              });

    Array<v2> res_points(1 + points.count);

    res_points.Push(points[0]);
    res_points.Push(points[1]);

    // Graham scan main loop
    usize i = 2;
    while (i < points.count) {
        if (vec2::IsLeft(
                points[i], res_points[res_points.count - 2], res_points[res_points.count - 1])) {
            res_points.Push(points[i]);
            i++;
        } else {
            res_points.count--;
        }
    }

    res_points.Push(points[0]);

    Array<Edge> result(res_points.count);

    for (usize j = 0; j < res_points.count - 1; ++j) {
        result.Push(Edge{res_points[j], res_points[j + 1]});
    }

    return result;
};