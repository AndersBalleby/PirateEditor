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

  // TODO -> Opstil en funktion som tager alle layouts og giver dem tiles ligesom her.
  for(size_t i = 0; i < layout.terrainLayout.size(); ++i) {
    for(size_t j = 0; j < layout.terrainLayout[i].size(); ++j) {
      int value = layout.terrainLayout[i][j];
      if(value != -1) {
        tiles.push_back(TileFactory::createTile(TILE_TYPE_TERRAIN, {(float) j, (float) i}, value));
      }
    }
  }

  for(size_t i = 0; i < layout.cratesLayout.size(); ++i) {
    for(size_t j = 0; j < layout.cratesLayout[i].size(); ++j) {
      int value = layout.cratesLayout[i][j];
      if(value != -1) {
        crates.push_back(TileFactory::createTile(TILE_TYPE_CRATE, {(float) j, (float) i}, value));
      }
    }
  }
}

void Scene::update(SDL_State& state) noexcept {
  bg.update(state);

  cameraX = 0.0f;
  if(state.keyState[SDL_SCANCODE_RIGHT] || state.keyState[SDL_SCANCODE_D]) cameraX += scrollSpeed * state.deltaTime;
  if(state.keyState[SDL_SCANCODE_LEFT] || state.keyState[SDL_SCANCODE_A]) cameraX -= scrollSpeed * state.deltaTime;

  float mapHeight = layout.terrainLayout.size() * TILE_SIZE;
  float mapOffsetY = state.windowHeight - mapHeight;
  if(mapOffsetY < 0) mapOffsetY = 0; // hvis vinduet er mindre end map

  for(auto& tile : tiles) {
    tile->dstRect.y = tile->position.y * TILE_SIZE + mapOffsetY + tile->offset.y;
    tile->update({cameraX, 0.0f});
  }

  for(auto& tile : crates) {
    tile->dstRect.y = tile->position.y * TILE_SIZE + mapOffsetY + tile->offset.y;
    tile->update({cameraX, 0.0f});
  }
};

void Scene::draw(SDL_Renderer* renderer) const noexcept {
  bg.render(renderer);

  for(const auto& tile : tiles) {
    tile->draw(renderer);
  }

  for(auto& tile : crates) {
    tile->draw(renderer);
  }
};

void Scene::handleInput(const SDL_Event &event, float deltaTime) noexcept {
  if(event.type == SDL_EVENT_KEY_DOWN) {
      if(event.key.key == SDLK_RIGHT) {
          cameraX += scrollSpeed * deltaTime; // scroll til højre
      } else if(event.key.key == SDLK_LEFT) {
          cameraX -= scrollSpeed * deltaTime; // scroll til venstre
      }
  }
}

void Scene::saveScene(const std::filesystem::path& path) {};
