#pragma once

#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "logging/Logger.hpp"
#include "core/ResourceManager.hpp"

class Cloud {
public:
    Cloud(Vec2<float> position);

    // Tillad flytning
    Cloud(Cloud&&) noexcept = default;
    Cloud& operator=(Cloud&&) noexcept = default;

    // Forbyd kopiering
    Cloud(const Cloud&) = delete;
    Cloud& operator=(const Cloud&) = delete;

    void update(float deltaTime);
    void render(SDL_Renderer* renderer) const;
    bool isActive() const;
private:
    Vec2<float> position;
    SDL_Texture* cloud_texture;
    SDL_FRect cloud_rect;
    bool active = true;
};

// Baggrunden håndterer også clouds
class Background {
public:
    Background();

    void update(float deltaTime) noexcept;
    void render(SDL_Renderer* renderer) const;
    void spawnCloud(Vec2<float> position);
private:
    float cloudTimer = 3.5f;
    std::vector<Cloud> clouds;
    SDL_FRect sky_top_rect;
    SDL_FRect sky_middle_rect;
    SDL_FRect sky_bottom_rect;

    SDL_Texture* sky_top;
    SDL_Texture* sky_middle;
    SDL_Texture* sky_bottom;
    bool init();
};
