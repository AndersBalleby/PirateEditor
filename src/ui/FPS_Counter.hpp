#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "logging/Logger.hpp"
#include <cstdint>

class FPS_Counter {
public:
    FPS_Counter();
    ~FPS_Counter();
    void update(SDL_Renderer* renderer, TTF_Font* font, float deltaTime) noexcept;
private:
    void updateText(SDL_Renderer* renderer, TTF_Font* font) noexcept;
    SDL_Texture* texture;
    SDL_FRect rect;
    std::uint64_t now;
    std::uint64_t last;
    double fpsTimer;
    int frameCount;
    int fps;
};
