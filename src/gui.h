#include <raylib.h>

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#endif
#include "data.h"
#include "types.h"

auto FullscreenToggle = [] {
    bool fullscreen = false;

    return [=]() mutable {
        if (GuiButton(Rectangle{10, 50, 100, 30}, "Fullscreen")) {
            ToggleBorderlessWindowed();

            if (!fullscreen) {
                i32 display                  = GetCurrentMonitor();
                settings.CurrentResolution.x = GetMonitorWidth(display);
                settings.CurrentResolution.y = GetMonitorHeight(display);
            } else {
                settings.CurrentResolution = settings.DefaultResolution;
            }

            SetWindowSize(settings.CurrentResolution.x, settings.CurrentResolution.y);
            fullscreen = !fullscreen;
        }
    };
}();