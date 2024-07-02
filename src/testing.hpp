#pragma once
#include <intrin.h>

#include <random>

#include "engine.hpp"

static Array<u64> gs_data(2000);
static Array<u64> jm_data(2000);
static Array<u64> ee_data(2000);

struct RayTesting : public Scene {
    Array<Shape2D> colliders{
        Rectangle{100, 100, 50, 50}, Rectangle{400, 300, 100, 50}, Rectangle{500, 50, 50, 100}};

    void DrawUI() final {
        for (usize i = 0; i < colliders.count; i++) {
            DrawRectangleLinesEx(colliders[i], 2.0f, BLACK);
        }

        v2 emitter = GetMousePosition();
        for (f32 i = 0; i < PI * 2 - .01; i += PI / 32) {
            v2        end{150 * cos(i) + emitter.x, 150 * sin(i) + emitter.y};
            Collision collision = CastRay(emitter, end, colliders);

            DrawLineV(emitter, collision.hit ? collision.point : end, GREEN);
            if (collision.hit)
                DrawCircleV(collision.point, 5, RED);
        }
        DrawCircleV(emitter, 10, BLUE);

        DrawFPS(10, 50);
    }
};

struct ConvexHullTesting : public Scene {
   private:
    const usize NUM = 2000;  // TODO CHANGE
    Array<v2>   test_points;
    Array<Edge> extremes;
    ItemGrabber grabber;

    Array<v2> generatePoints(const usize count) {
        static Array<v2> points(count);
        points.Clear();

        std::random_device                    rd;         // Obtain a random number from hardware
        std::mt19937                          eng(rd());  // Seed the generator
        std::uniform_real_distribution<float> distr(0.0, 1.0);  // Define the range

        for (usize i = 0; i < count; ++i) {
            points.Push(v2{distr(eng) * screenWidth / 2 + screenWidth / 5,
                           distr(eng) * screenHeight / 2 + screenHeight / 5});
        }

        return points;
    }

    void drawPoints(const Array<v2>& points) {
        for (usize i = 0; i < points.count; ++i) {
            DrawCircle(points[i].x, points[i].y, 5, BLACK);

            std::string display = std::format("{:.0f}, {:.0f} [{}]", points[i].x, points[i].y, i);
            DrawText(display.c_str(), points[i].x + 5, points[i].y + 5, 10, BLACK);

            Circle welzl = EnclosingDisk(points);
            DrawCircleLines(welzl.x, welzl.y, welzl.r, BLUE);
        }
    }

    void drawEdges(const Array<Edge>& extremes) {
        for (usize i = 0; i < extremes.count; i++) {
            DrawLine(extremes[i].p.x, extremes[i].p.y, extremes[i].q.x, extremes[i].q.y, BLUE);
        }
    }

   public:
    ConvexHullTesting()
        : test_points{generatePoints(NUM)},
          extremes{ConvexHull_GrahamScan(test_points)},
          grabber{ItemGrabber(&test_points)} {}

    void Compute() final {
        // grabber();

        u64 st   = __rdtsc();
        extremes = ConvexHull_GrahamScan(test_points);
        st       = __rdtsc() - st;
        gs_data.Push(st);

        st = __rdtsc();
        ConvexHull_JarvisMarch(test_points);
        st = __rdtsc() - st;
        jm_data.Push(st);

        st = __rdtsc();
        ConvexHull_ExtremeEdges(test_points);
        st = __rdtsc() - st;
        ee_data.Push(st);
    }

    void Draw2D() final {}

    void DrawUI() final {
        drawPoints(test_points);
        drawEdges(extremes);

        if (GuiButton(Rectangle{10, 50, 100, 30}, "New points")) {
            test_points = generatePoints(NUM);
        }

        DrawFPS(10, 100);
    }

    void PerPointsTest() {
        for (usize i = 2; i < 999; i++) {
            std::cout << "PROCESSING TEST " << i << "\n";
            auto inc_points = generatePoints(i);
            u64  st          = __rdtsc();
            extremes         = ConvexHull_GrahamScan(inc_points);
            st               = __rdtsc() - st;
            gs_data.Push(st);

            st = __rdtsc();
            ConvexHull_JarvisMarch(inc_points);
            st = __rdtsc() - st;
            jm_data.Push(st);

            st = __rdtsc();
            ConvexHull_ExtremeEdges(inc_points);
            st = __rdtsc() - st;
            ee_data.Push(st);
        }
    }
};