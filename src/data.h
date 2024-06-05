#pragma once

#include <raylib.h>

#include "types.h"

struct Settings {
    v2 DefaultResolution{1024, 600};
    v2 CurrentResolution = DefaultResolution;
};
auto settings = Settings();

struct {
    Array<Model> models = Array<Model>(256);
} data = {};

Texture tileset;

struct {
    CameraMode mode = CAMERA_FREE;
    Camera3D   camera3D{.position   = {10.0f, 10.0f, 10.0f},
                        .target     = {0.0f, 0.0f, 0.0f},
                        .up         = {0.0f, 1.0f, 0.0f},
                        .fovy       = 45.0f,
                        .projection = CAMERA_PERSPECTIVE};

    operator Camera3D() const { return camera3D; }

    void Update() { UpdateCamera(&camera3D, mode); }

    void UpdatePro() { UpdateCameraPro(&camera3D, v3{}, v3{}, 0); }
} camera3D{};

struct {
    Camera2D    camera2D{};
    Damped<v2>  dampedOffset{};
    Damped<f32> dampedZoom{};

    operator Camera2D() const { return camera2D; }

    void Update() {
        // TODO Notice how there's such a thing as a freezing damper and one with inertia.
        // We'd never thought of this before, huh?

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