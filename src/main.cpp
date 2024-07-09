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
        new TileEditor("D:\\Dev\\Motley\\assets\\tilesets\\MRMOTEXT-EX.png", 8, 32)};
    static usize current = 0;

    if (IsKeyPressed(KEY_SPACE)) {
        current = (current + 1) % Scenes.count;
    }

    Scene* scene = Scenes[current];

    scene->Compute();
    camera2D.Update();

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
        printf_s("INFO: ENGINE: Saving screen as image\n");
        TakeScreenshot("image.png");
    }

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        BeginMode2D(camera2D);
        scene->Draw2D();
        EndMode2D();

        scene->DrawUI();
        FullscreenToggle();
    }
    EndDrawing();
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "EngineTest");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        Update();
    }

    CloseWindow();
}