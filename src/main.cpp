#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else  // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#include "textmode.hpp"

void Update() {
    static Array<Scene*> Scenes{
        new MainMenu(),
        new TileEditor("D:\\Dev\\Motley\\assets\\tilesets\\MRMOTEXT-EX.png", 8, 32, v2{40, 20})};
    static usize current = 0;

    if (IsKeyPressed(KEY_SPACE)) {
        current = (current + 1) % Scenes.count;
    }

    Scene* scene = Scenes[current];

    scene->Compute();

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        BeginMode2D(scene->camera);
        scene->Draw2D();
        EndMode2D();

        scene->DrawUI();
    }
    EndDrawing();
}

int main() {
    // SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "EngineTest");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Update();
    }

    CloseWindow();
}