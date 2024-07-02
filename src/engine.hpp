#pragma once

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include "algorithm.hpp"

struct MyCamera2D {
    Camera2D    camera2D{};
    Damped<v2>  dampedOffset{};
    Damped<f32> dampedZoom{};

    operator Camera2D() const { return camera2D; }

    void Update() {
        // TODO Notice how there's such a thing as a freezing damper vs one with inertia.

        camera2D.offset = dampedOffset.By(v2{IsKeyDown(KEY_RIGHT)  ? -5.0f
                                             : IsKeyDown(KEY_LEFT) ? 5.0f
                                                                   : 0,
                                             IsKeyDown(KEY_UP)     ? 5.0f
                                             : IsKeyDown(KEY_DOWN) ? -5.0f
                                                                   : 0});

        if (IsKeyDown(KEY_A))
            camera2D.rotation--;
        else if (IsKeyDown(KEY_S))
            camera2D.rotation++;

        if (camera2D.rotation > 40)
            camera2D.rotation = 40;
        else if (camera2D.rotation < -40)
            camera2D.rotation = -40;

        camera2D.zoom = dampedZoom.By((float)GetMouseWheelMove() * 0.15f);

        if (camera2D.zoom > 3.0f)
            camera2D.zoom = 3.0f;
        else if (camera2D.zoom < 0.1f)
            camera2D.zoom = 0.1f;

        if (IsKeyPressed(KEY_R)) {
            camera2D.zoom     = dampedZoom(1.0f);
            camera2D.rotation = 0.0f;
        }
    }
} camera2D{};

static u32 screenWidth  = 800;
static u32 screenHeight = 450;
void       FullscreenToggle() {
    static bool fullscreen = false;

    if (GuiButton(Rectangle{10, 10, 100, 30}, "Fullscreen")) {
        ToggleBorderlessWindowed();

        if (!fullscreen) {
            i32 display  = GetCurrentMonitor();
            screenWidth  = GetMonitorWidth(display);
            screenHeight = GetMonitorHeight(display);
        } else {
            screenWidth  = 800;
            screenHeight = 450;
        }

        SetWindowSize(screenWidth, screenHeight);
        fullscreen = !fullscreen;
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

struct Scene {
    Scene() {}

    virtual void Compute() {}

    virtual void Draw2D() {}

    virtual void DrawUI() {}
};

struct CharGrid {
    const u32 width, height;
    Texture   charSheet;
    u8        charResolution;
    u8*       chars;

    CharGrid(u32 _width, u32 _height)
        : width{_width}, height{_height}, chars{(u8*)malloc(width * height)} {}

    CharGrid(u32 _width) : width{_width}, height{_width}, chars{(u8*)malloc(width * height)} {}

    void Set(u8 x, u8 y, u8 to) { chars[x * y] = to; }
};