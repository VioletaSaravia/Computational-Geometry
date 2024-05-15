#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
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
#include <random>

Array<v2> GeneratePoints(const usize count) {
    Array<v2> points(count);

    std::random_device                    rd;               // Obtain a random number from hardware
    std::mt19937                          eng(rd());        // Seed the generator
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

        auto display = std::format("{:.0f}, {:.0f} [{}]", points[i].x, points[i].y, i);
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

const usize NUM         = 20;
Array<v2>   test_points = GeneratePoints(NUM);
Array<Edge> extremes    = ConvexHull_GrahamScan(test_points);

void ConvexHull_Test() {
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
}
// --------------------------------------------

void Update();

struct GameData {
    Array<Model> models = Array<Model>(256);
};
auto data = GameData{};

Camera3D camera{};
i32      main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(settings.CurrentResolution.x, settings.CurrentResolution.y, "Raylib Test");

    data.models.Push(LoadModel("D:\\Dev\\RaylibTesting\\assets\\cube\\default_cube.obj"));
    auto cubeTexture = LoadTexture("D:\\Dev\\RaylibTesting\\assets\\cube\\container.jpg");
    data.models[0].materials[0].maps[MATERIAL_MAP_ALBEDO].texture = cubeTexture;

    camera.position   = {10.0f, 10.0f, 10.0f};  // Camera position
    camera.target     = {0.0f, 0.0f, 0.0f};     // Camera looking at point
    camera.up         = {0.0f, 1.0f, 0.0f};     // Camera up vector (rotation towards target)
    camera.fovy       = 45.0f;                  // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;     // Camera projection type

    auto litShader = LoadShader("D:/Dev/RaylibTesting/shaders/default.vert",
                                "D:/Dev/RaylibTesting/shaders/default_lit.frag");

    while (!IsShaderReady(litShader));

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(Update, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) Update();
#endif

    CloseWindow();
    return 0;
}

bool Fullscreen = false;
void FullscreenToggle() {
    if (GuiButton(Rectangle{10, 50, 100, 30}, "Fullscreen")) {
        ToggleBorderlessWindowed();

        if (!Fullscreen) {
            i32 display                  = GetCurrentMonitor();
            settings.CurrentResolution.x = GetMonitorWidth(display);
            settings.CurrentResolution.y = GetMonitorHeight(display);
        } else {
            settings.CurrentResolution = settings.DefaultResolution;
        }

        SetWindowSize(settings.CurrentResolution.x, settings.CurrentResolution.y);
        Fullscreen = !Fullscreen;
    }
}

void Update() {
    UpdateCamera(&camera, CAMERA_FREE);
    BeginDrawing();
    {
        FullscreenToggle();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        {
            DrawGrid(10, 1.0f);
            DrawModel(data.models[0], {0, 0, 0}, 1, WHITE);
        }
        EndMode3D();

        ConvexHull_Test();
    }
    EndDrawing();
}
