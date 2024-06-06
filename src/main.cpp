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

template <typename T>
struct FileWatcher {
    const std::string basePath{};

    std::vector<T>           loadedFiles;
    std::vector<std::string> filePaths;

    std::vector<fs::file_time_type>               lastModified;
    std::function<T(fs::directory_entry const &)> loader;
    std::function<void(T &)>                      unloader;

    FileWatcher(std::string                                   _basePath,
                std::function<T(fs::directory_entry const &)> _loader,
                std::function<void(T &)>                      _unloader)
        : basePath{_basePath}, loader{_loader}, unloader{_unloader} {}

    void Init() {
        usize i = 0;
        for (auto const &file : fs::directory_iterator(basePath)) {
            filePaths.push_back(file.path());
            loadedFiles.push_back(loader(file));
            lastModified.push_back(file.last_write_time());
            i++;
        }
    }

    void Update() {
        usize i = 0;
        for (auto &file : fs::directory_iterator(basePath)) {
            auto currentModified = file.last_write_time();
            if (currentModified != lastModified[i]) {
                std::cout << std::format("INFO: Reloading modified file: {}\n",
                                         file.path().c_str());

                unloader(loadedFiles[i]);
                loadedFiles[i]  = loader(file);
                lastModified[i] = currentModified;
            }
            i++;
        }
    }
};

static FileWatcher<Shader> shaderWatcher{
    "/home/violeta/Dev/RaylibTesting/shaders/fragment",
    [](fs::directory_entry const &file) { return LoadShader(NULL, file.path().c_str()); },
    [](Shader &shader) { UnloadShader(shader); }};

static FileWatcher<i32> mockWatcher{
    "/home/violeta/Dev/RaylibTesting/assets/tilesets/",
    []([[maybe_unused]] fs::directory_entry const &file) { return 0; },
    []([[maybe_unused]] i32 &shader) { return; }};

void Update() {
    camera3D.Update();
    camera2D.Update();
    shaderWatcher.Update();
    mockWatcher.Update();

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

    shaderWatcher.Init();
    mockWatcher.Init();

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
