#include "Scene.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "core/ResourceManager.hpp"
#include "core/TileManager.hpp"

SceneLayout::SceneLayout(unsigned int level) {
  bgPalmsLayout     = LoadLevelLayout(level, "bg_palms");
  coinsLayout       = LoadLevelLayout(level, "coins");
  constraintsLayout = LoadLevelLayout(level, "constraints");
  cratesLayout      = LoadLevelLayout(level, "crates");
  enemiesLayout     = LoadLevelLayout(level, "enemies");
  fgPalmsLayout     = LoadLevelLayout(level, "fg_palms");
  grassLayout       = LoadLevelLayout(level, "grass");
  playerLayout      = LoadLevelLayout(level, "player");
  terrainLayout     = LoadLevelLayout(level, "terrain");
}

Utils::Layout SceneLayout::LoadLevelLayout(unsigned int level, const std::string& name) {
  Utils::Layout result = Utils::LoadCSVFile(std::format("levels/{0}/level_{0}_{1}.csv", level, name));
  if(result.empty()) {
    Log::Error("Kunne ikke validere resultat fra Utils::LoadCSVFile ({})", name);
    return {};
  }

  return result;
}

Scene::Scene(unsigned int level, const std::string& name)
  : level(level)
  , name(name)
  , layout(SceneLayout(level))
{
  Log::Info("Indlæste scene \"{}\" successfuldt", name);
  for(size_t i = 0; i < layout.terrainLayout.size(); ++i) {
    for(size_t j = 0; j < layout.terrainLayout[i].size(); ++j) {
      tiles.push_back(TileFactory::createTile(TILE_TYPE_TERRAIN, {(float) j, (float) i}, layout.terrainLayout[i][j]));
    }
  }
}

void Scene::update(float deltaTime, const bool* keyState) noexcept {
  bg.update(deltaTime);

  cameraX = 0.0f;
  if(keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) cameraX += scrollSpeed * deltaTime;
  if(keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) cameraX -= scrollSpeed * deltaTime;

  for(auto& tile : tiles) {
    tile->update(cameraX);
  }
};

void Scene::draw(SDL_Renderer* renderer) const noexcept {
  bg.render(renderer);

  for(const auto& tile : tiles) {
    tile->draw(renderer);
  }
};

void Scene::handleInput(const SDL_Event &event, float deltaTime) noexcept {
  if(event.type == SDL_EVENT_KEY_DOWN) {
      if(event.key.key == SDLK_RIGHT) {
          cameraX += scrollSpeed * deltaTime; // scroll til højre
      }
      else if(event.key.key == SDLK_LEFT) {
          cameraX -= scrollSpeed * deltaTime; // scroll til venstre
      }
  }
}

void Scene::saveScene(const std::filesystem::path& path) {};
