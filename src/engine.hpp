#pragma once

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include "algorithm.hpp"

struct SmoothCamera2D {
    Camera2D camera{};
    // TODO These should be clamped as well.
    Damped<v2>  dampedOffset{};
    Damped<f32> dampedZoom{1.0f, false};
    Damped<f32> dampedRotation{};

    v2  offsetClamp[2]   = {{FLT_MIN, FLT_MAX}, {FLT_MIN, FLT_MAX}};
    f32 rotationClamp[2] = {-40, 40};
    f32 zoomClamp[2]     = {0.001f, 30.0f};

    operator Camera2D() const { return camera; }

    void Update() {
        camera.rotation = dampedRotation.Set();

        MiddleMouseMovementAndZoom();

        Clamp();
    }

    void Clamp() {
        if (camera.rotation > rotationClamp[1])
            camera.rotation = rotationClamp[1];
        else if (camera.rotation < rotationClamp[0])
            camera.rotation = rotationClamp[0];

        if (camera.zoom > zoomClamp[1])
            camera.zoom = zoomClamp[1];
        else if (camera.zoom < zoomClamp[0])
            camera.zoom = zoomClamp[0];

        // if (camera.offset.x > offsetClamp[1].x)
        //     camera.offset.x = offsetClamp[1].x;
        // else if (camera.offset.x < offsetClamp[0].x)
        //     camera.offset.x = offsetClamp[0].x;

        // if (camera.offset.y > offsetClamp[1].y)
        //     camera.offset.y = offsetClamp[1].y;
        // else if (camera.offset.y < offsetClamp[0].y)
        //     camera.offset.y = offsetClamp[0].y;
    }

    void MiddleMouseMovementAndZoom() {
        camera.zoom = dampedZoom.By((float)GetMouseWheelMove() * 0.15f);

        static f32 buttonHeld{};
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            buttonHeld += GetFrameTime();
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) && buttonHeld < 0.200f) {
            camera.zoom     = dampedZoom.Set(1.0f);
            camera.rotation = dampedRotation.Set(0.0f);
            buttonHeld      = 0;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
            buttonHeld = 0;
        }

        camera.offset = dampedOffset.By(
            IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) && buttonHeld >= 0.200f ? GetMouseDelta()
                                                                           : v2{});
    }

    v2 Reproject(const v2& vector) {
        return (vector + camera.target - camera.offset) / camera.zoom;
    }
} camera{};

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
            *held       = newHeld.Set(mousePos);
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