#define DEFAULT_ARENA_SIZE 10000

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include "algorithm.h"
#include "damped.h"
#include "data.h"
#include "geometry2d.h"
#include "gui.h"
#include "testing.h"
#include "types.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else  // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

void Update() {
    camera3D.Update();
    camera2D.Update();

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera3D);
        {}
        EndMode3D();

        BeginMode2D(camera2D);
        {}
        EndMode2D();

        FullscreenToggle();
        convexHull_Test();
    }
    EndDrawing();
}

i32 main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(settings.CurrentResolution.x, settings.CurrentResolution.y, "Raylib Test");

    data.models.Push(LoadModel("/home/violeta/Dev/RaylibTesting/assets/cube/default_cube.obj"));
    auto cubeTexture = LoadTexture("/home/violeta/Dev/RaylibTesting/assets/cube/container.jpg");
    data.models[0].materials[0].maps[MATERIAL_MAP_ALBEDO].texture = cubeTexture;

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
