#define DEFAULT_ARENA_SIZE 10000

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include "algorithm.h"
#include "data.h"
#include "geometry2d.h"
#include "types.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else  // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

// ---------------- TESTING -------------------
#include <format>
#include <functional>
#include <iostream>
#include <random>

#include "damped.h"

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

auto ConvexHull_Test = []() {
    const usize NUM         = 20;
    Array<v2>   test_points = GeneratePoints(NUM);
    Array<Edge> extremes    = ConvexHull_GrahamScan(test_points);

    return [=]() mutable {
        if (GuiButton(Rectangle{10, 10, 100, 30}, "New points")) {
            test_points = GeneratePoints(NUM);
        }
        extremes = ConvexHull_GrahamScan(test_points);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            v2 mousePos = GetMousePosition();
            for (usize i = 0; i < test_points.count; i++) {
                if (vec2::DistanceTo(mousePos, test_points[i]) <= 20) {
                    test_points[i] = mousePos;
                    break;
                }
            }
        }

        DrawPoints(test_points);
        DrawEdges(extremes);
    };
}();
// --------------------------------------------

void Update();

struct {
    Array<Model> models = Array<Model>(256);
} data = {};

Texture  tileset;
Camera3D camera3D{};
Camera2D camera2D{};

i32 main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(settings.CurrentResolution.x, settings.CurrentResolution.y, "Raylib Test");

    data.models.Push(LoadModel("/home/violeta/Dev/RaylibTesting/assets/cube/default_cube.obj"));
    auto cubeTexture = LoadTexture("/home/violeta/Dev/RaylibTesting/assets/cube/container.jpg");
    data.models[0].materials[0].maps[MATERIAL_MAP_ALBEDO].texture = cubeTexture;

    camera3D.position   = {10.0f, 10.0f, 10.0f};  // Camera position
    camera3D.target     = {0.0f, 0.0f, 0.0f};     // Camera looking at point
    camera3D.up         = {0.0f, 1.0f, 0.0f};     // Camera up vector (rotation towards target)
    camera3D.fovy       = 45.0f;                  // Camera field-of-view Y
    camera3D.projection = CAMERA_PERSPECTIVE;     // Camera projection type

    tileset = LoadTexture("/home/violeta/Dev/RaylibTesting/assets/tilesets/microbe-2.png");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(Update, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) Update();
#endif

    CloseWindow();
    return 0;
}

// Toggles fullscreen on/off.
auto Fullscreen = [] {
    bool fullscreen = false;

    return [=]() mutable {
        if (GuiButton(Rectangle{10, 50, 100, 30}, "Fullscreen")) {
#if defined(PLATFORM_WEB)
// emscripten_request_fullscreen("???", false);
#endif
            ToggleBorderlessWindowed();

            if (!fullscreen) {
                i32 display                  = GetCurrentMonitor();
                settings.CurrentResolution.x = GetMonitorWidth(display);
                settings.CurrentResolution.y = GetMonitorHeight(display);
            } else {
                settings.CurrentResolution = settings.DefaultResolution;
            }

            SetWindowSize(settings.CurrentResolution.x, settings.CurrentResolution.y);
            fullscreen = !fullscreen;
        }
    };
}();

Array<Array<v2>> example(3, Array<v2>(3, v2{2, 3}));

Damped<v2> dampedOffset{};

Damped<f32> dampedZoom{};

void Update() {
    UpdateCamera(&camera3D, CAMERA_FREE);

    // TODO Notice how there's such a thing as a freezing damper and one with inertia.
    // We'd never thought of this before, huh?

    camera2D.offset = dampedOffset.By(v2{IsKeyDown(KEY_RIGHT)  ? -5.0f
                                         : IsKeyDown(KEY_LEFT) ? 5.0f
                                                               : 0,
                                         IsKeyDown(KEY_UP)     ? 5.0f
                                         : IsKeyDown(KEY_DOWN) ? -5.0f
                                                               : 0});

    // Camera rotation controls
    if (IsKeyDown(KEY_A))
        camera2D.rotation--;
    else if (IsKeyDown(KEY_S))
        camera2D.rotation++;

    // Limit camera2D rotation to 80 degrees (-40 to 40)
    if (camera2D.rotation > 40)
        camera2D.rotation = 40;
    else if (camera2D.rotation < -40)
        camera2D.rotation = -40;

    // camera2D zoom controls
    camera2D.zoom = dampedZoom.By((float)GetMouseWheelMove() * 0.15f);

    if (camera2D.zoom > 3.0f)
        camera2D.zoom = 3.0f;
    else if (camera2D.zoom < 0.1f)
        camera2D.zoom = 0.1f;

    // camera2D reset (zoom and rotation)
    if (IsKeyPressed(KEY_R)) {
        camera2D.zoom     = dampedZoom(1.0f);
        camera2D.rotation = 0.0f;
    }

    BeginDrawing();
    {
        Fullscreen();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera2D);
        {
            for (usize i = 0; i < example.count; i++)
                for (usize j = 0; j < example[i].count; j++)
                    DrawTexturePro(tileset,
                                   Rectangle{6 * example[i][j].x, 6 * example[i][j].y, 6, 6},
                                   Rectangle{f32(200 + i * 24), f32(200 + j * 24), 24, 24},
                                   v2{100, 100},
                                   0.0f,
                                   WHITE);

            // ConvexHull_Test();
        }

        BeginMode3D(camera3D);
        {
            //     DrawGrid(10, 1.0f);
            //     DrawModel(data.models[0], {0, 0, 0}, 1, WHITE);
        }
        EndMode3D();
    }
    EndDrawing();
}
