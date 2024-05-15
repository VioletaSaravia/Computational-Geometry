#pragma once

#include <raylib.h>

#include "types.h"

struct Settings {
    v2 DefaultResolution{1024, 600};

    v2 CurrentResolution = DefaultResolution;
};
auto settings = Settings();