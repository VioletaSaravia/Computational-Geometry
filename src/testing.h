#include <raylib.h>

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#endif

#include <format>
#include <functional>
#include <iostream>
#include <random>

#include "algorithm.h"
#include "data.h"
#include "geometry2d.h"

static Arena PointsArena(DEFAULT_ARENA_SIZE);
Array<v2>    GeneratePoints(const usize count) {
    Array<v2> points(count, &PointsArena);

    std::random_device                    rd;         // Obtain a random number from hardware
    std::mt19937                          eng(rd());  // Seed the generator
    std::uniform_real_distribution<float> distr(0.0, 1.0);  // Define the range

    for (usize i = 0; i < count; ++i) {
        points.Push(
            v2{distr(eng) * settings.CurrentResolution.x / 2 + settings.CurrentResolution.x / 5,
               distr(eng) * settings.CurrentResolution.y / 2 + settings.CurrentResolution.y / 5});
    }

    return points;
}

void DrawPoints(const Array<v2>& points) {
    for (usize i = 0; i < points.count; ++i) {
        DrawCircle(points[i].x, points[i].y, 5, BLACK);

        std::string display = std::format("{:.0f}, {:.0f} [{}]", points[i].x, points[i].y, i);
        DrawText(display.c_str(), points[i].x + 5, points[i].y + 5, 10, BLACK);

        Circle welzl = EnclosingDisk(points);
        DrawCircleLines(welzl.x, welzl.y, welzl.r, BLUE);
    }
}

void DrawEdges(const Array<Edge>& extremes) {
    for (usize i = 0; i < extremes.count; i++) {
        DrawLine(extremes[i].p.x, extremes[i].p.y, extremes[i].q.x, extremes[i].q.y, BLUE);
    }
}

struct ItemGrabber {
    Array<v2>* items;
    v2*        held = nullptr;
    Damped<v2> newHeld{};

    void operator()() {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            std::cout << "RELEASED\n";
            held    = nullptr;
            newHeld = Damped<v2>{};
            return;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            std::cout << "PRESSED\n";

            v2 mousePos = GetMousePosition();
            for (usize i = 0; i < items->count; i++) {
                if (vec2::DistanceTo(mousePos, (*items)[i]) <= 20) {
                    held  = &(*items)[i];
                    *held = mousePos;
                    return;
                }
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && held) {
            std::cout << "DOWN\n";

            v2 mousePos = GetMousePosition();
            *held       = newHeld(mousePos);
        }
    }
};

struct ConvexHull_Test {
    const usize NUM         = 20;
    Array<v2>   test_points = GeneratePoints(NUM);
    Array<Edge> extremes    = ConvexHull_GrahamScan(test_points);
    ItemGrabber grabber     = ItemGrabber(&test_points);

    void operator()() {
        // grabber();

        if (GuiButton(Rectangle{10, 10, 100, 30}, "New points")) {
            test_points = GeneratePoints(NUM);
        }

        extremes = ConvexHull_GrahamScan(test_points);

        DrawPoints(test_points);
        DrawEdges(extremes);
    }
};
auto convexHull_Test = ConvexHull_Test();