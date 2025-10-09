#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "sdl/SDL_Handler.hpp"
#include "logging/Logger.hpp"
#include <cstdint>

class FPS_Counter {
public:
    FPS_Counter();
    ~FPS_Counter() = default;
    void update(SDL_State& state) noexcept;
private:
    SDL_FRect rect;
    std::uint64_t now;
    std::uint64_t last;
    double fpsTimer;
    int frameCount;
    int fps;
};
