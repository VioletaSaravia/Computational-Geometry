#define DEFAULT_ARENA_SIZE 10000

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include <chrono>
#include <filesystem>

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

namespace fs = std::filesystem;
namespace ch = std::chrono;

struct ShaderLoader {
    const std::string basePath{"/home/violeta/Dev/RaylibTesting/shaders/fragment"};

    std::vector<Shader>      shaders;
    std::vector<std::string> files;

    std::vector<fs::file_time_type> lastModified;
    ch::duration<i32, std::milli>   watchDelay = ch::milliseconds(100);

    ShaderLoader() {}

    void Init() {
        usize i = 0;
        for (auto const &file : fs::directory_iterator(basePath)) {
            files.push_back(file.path());
            shaders.push_back(LoadShader(NULL, file.path().c_str()));
            lastModified.push_back(file.last_write_time());
            i++;
        }
    }

    void Update() {
        usize i = 0;
        for (auto &file : fs::directory_iterator(basePath)) {
            auto currentModified = file.last_write_time();
            if (currentModified != lastModified[i]) {
                std::cout << std::format("INFO: SHADER: [ID {}] Reloading modified file: {}\n",
                                         shaders[i].id,
                                         file.path().c_str());

                UnloadShader(shaders[i]);
                shaders[i]      = LoadShader(NULL, files[i].c_str());
                lastModified[i] = currentModified;
            }
            i++;
        }
    }
};
static ShaderLoader shaderLoader{};

void Update() {
    camera3D.Update();
    camera2D.Update();
    shaderLoader.Update();

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

    shaderLoader.Init();
    
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
