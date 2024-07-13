#pragma once

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include "algorithm.hpp"

struct BaseCamera2D : public Camera2D {
    v2 Reproject(const v2& vector) { return (vector + target - offset) / zoom; }

    virtual void Update() {};
};

struct PixelCamera2D : public BaseCamera2D {
    PixelCamera2D() {};

    void Update() final {}
};

struct SmoothCamera2D : public BaseCamera2D {
    // TODO These should be clamped as well.
    Damped<v2>  dampedOffset{};
    Damped<f32> dampedZoom{1.0f, false};
    Damped<f32> dampedRotation{};

    v2  offsetClamp[2]   = {{FLT_MIN, FLT_MAX}, {FLT_MIN, FLT_MAX}};
    f32 rotationClamp[2] = {-40, 40};
    f32 zoomClamp[2]     = {0.001f, 30.0f};

    void Update() final {
        rotation = dampedRotation.Set();

        MiddleMouseMovementAndZoom();

        Clamp();
    }

    void Clamp() {
        if (rotation > rotationClamp[1])
            rotation = rotationClamp[1];
        else if (rotation < rotationClamp[0])
            rotation = rotationClamp[0];

        if (zoom > zoomClamp[1])
            zoom = zoomClamp[1];
        else if (zoom < zoomClamp[0])
            zoom = zoomClamp[0];

        // if (offset.x > offsetClamp[1].x)
        //     offset.x = offsetClamp[1].x;
        // else if (offset.x < offsetClamp[0].x)
        //     offset.x = offsetClamp[0].x;

        // if (offset.y > offsetClamp[1].y)
        //     offset.y = offsetClamp[1].y;
        // else if (offset.y < offsetClamp[0].y)
        //     offset.y = offsetClamp[0].y;
    }

    void MiddleMouseMovementAndZoom() {
        zoom = dampedZoom.By((float)GetMouseWheelMove() * 0.15f);

        static f32 buttonHeld{};
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            buttonHeld += GetFrameTime();
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) && buttonHeld < 0.200f) {
            zoom       = dampedZoom.Set(1.0f);
            rotation   = dampedRotation.Set(0.0f);
            buttonHeld = 0;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
            buttonHeld = 0;
        }

        offset = dampedOffset.By(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) && buttonHeld >= 0.200f
                                     ? GetMouseDelta()
                                     : v2{});
    }
};

static u32 screenWidth  = 1024;
static u32 screenHeight = 768;
void       FullscreenToggle() {
    static bool fullscreen = false;

    if (GuiButton(Rectangle{10, 10, 100, 30}, "Fullscreen")) {
        ToggleBorderlessWindowed();

        if (!fullscreen) {
            i32 display  = GetCurrentMonitor();
            screenWidth  = GetMonitorWidth(display);
            screenHeight = GetMonitorHeight(display);
        } else {
            screenWidth  = 1024;
            screenHeight = 768;
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
    BaseCamera2D camera{};

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