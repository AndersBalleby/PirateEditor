#include "Background.hpp"
#include "SDL3/SDL_render.h"
#include "core/ResourceManager.hpp"
#include "logging/Logger.hpp"

namespace fs = std::filesystem;

//
// CLOUD CLASS
//
static const fs::path CLOUDS[3] = {
    fs::path("resources/decoration/clouds/1.png"),
    fs::path("resources/decoration/clouds/2.png"),
    fs::path("resources/decoration/clouds/3.png")
};

Cloud::Cloud(Vec2<float> position) {
  // Random cloud texture
  int random = rand() % 3;
  this->cloud_texture = ResourceManager::loadTexture(CLOUDS[random]);

  if(this->cloud_texture == nullptr) {
    Log::Error("Kunne ikke indlæse sky ved position ({}, {})", position.x, position.y);
    return;
  }

    this->position = position;
    this->cloud_rect = {position.x, position.y, (float)cloud_texture->w, (float)cloud_texture->h};
    Log::Info("Oprettet sky ved ({}, {})", position.x, position.y);
}

void Cloud::update(float deltaTime) {
  if(active) {
    this->position.x -= deltaTime * 100;
    if(this->position.x < -cloud_texture->w) {
      this->active = false;
      Log::Info("Sky ved ({}, {}) blev fjernet", position.x, position.y);
    }
    this->cloud_rect.x = this->position.x;
  }
}

void Cloud::render(SDL_Renderer *renderer) const{
  if(active)
    SDL_RenderTexture(renderer, cloud_texture, nullptr, &cloud_rect);
}

bool Cloud::isActive() const {
  return active;
}

//
// BACKGROUND CLASS
//
Background::Background() {
  if(!init()) {
    Log::Error("Kunne ikke initialisere baggrund");
  }

  Log::Info("Baggrund initialiseret");
}

static const fs::path SKY_BOTTOM_PATH = fs::path("resources/decoration/sky/sky_bottom.png");
static const fs::path SKY_MIDDLE_PATH = fs::path("resources/decoration/sky/sky_middle.png");
static const fs::path SKY_TOP_PATH = fs::path("resources/decoration/sky/sky_top.png");
static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 800;
bool Background::init() {
  Log::Info("Initialiserer baggrund");
  sky_bottom = ResourceManager::loadTexture(SKY_BOTTOM_PATH);
  sky_middle = ResourceManager::loadTexture(SKY_MIDDLE_PATH);
  sky_top = ResourceManager::loadTexture(SKY_TOP_PATH);

  if(!sky_bottom || !sky_middle || !sky_top) {
    Log::Error("Kunne ikke indlæse baggrundsbilleder");
    return false;
  }

  sky_top_rect = {0, 0, (float)WINDOW_WIDTH, 200};
  sky_middle_rect = {0, 200, (float)WINDOW_WIDTH, 400};
  sky_bottom_rect = {0, 600, (float)WINDOW_WIDTH, 200};
  return true;
}

void Background::spawnCloud(Vec2<float> position) {
  clouds.emplace_back(Cloud(position));
}

static const float CLOUD_SPAWN_INTERVAL = 5.0f;
void Background::update(float deltaTime) noexcept {
  if(cloudTimer >= CLOUD_SPAWN_INTERVAL) {
    spawnCloud({(float) WINDOW_WIDTH, (float)(rand() % (WINDOW_HEIGHT - 650))});
    cloudTimer = 0.0f;
  }

  cloudTimer += deltaTime;

  // Opdater skyer
  for(auto& cloud : clouds) {
    cloud.update(deltaTime);
  }

  // Fjern inaktive skyer
  clouds.erase(
      std::remove_if(clouds.begin(), clouds.end(),
          [](const Cloud& c) { return !c.isActive(); }),
      clouds.end()
  );
}

void Background::render(SDL_Renderer *renderer) const {
    // Render background
    // Render top
    SDL_RenderTexture(renderer, sky_top, nullptr, &sky_top_rect);

    // Render middle
    SDL_RenderTexture(renderer, sky_middle, nullptr, &sky_middle_rect);

    // Render bottom
    SDL_RenderTexture(renderer, sky_bottom, nullptr, &sky_bottom_rect);

    // Render clouds
    for(const auto& cloud : clouds) {
      cloud.render(renderer);
    }
}
