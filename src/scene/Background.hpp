#pragma once

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "logging/Logger.hpp"
#include "core/ResourceManager.hpp"

namespace fs = std::filesystem;

class Background {
public:
    Background();

    void render(SDL_Renderer* renderer);
private:
    const fs::path SKY_BOTTOM_PATH = fs::path("resources/decoration/sky/sky_bottom.png");
    const fs::path SKY_MIDDLE_PATH = fs::path("resources/decoration/sky/sky_middle.png");
    const fs::path SKY_TOP_PATH = fs::path("resources/decoration/sky/sky_top.png");
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 800;

    SDL_FRect sky_top_rect = {0, 0, (float)WINDOW_WIDTH, 200};
    SDL_FRect sky_middle_rect = {0, 200, (float)WINDOW_WIDTH, 400};
    SDL_FRect sky_bottom_rect = {0, 600, (float)WINDOW_WIDTH, 200};

    SDL_Texture* sky_top;
    SDL_Texture* sky_middle;
    SDL_Texture* sky_bottom;
    bool init();
};
