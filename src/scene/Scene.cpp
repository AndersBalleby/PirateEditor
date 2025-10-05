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

SceneTiles::SceneTiles(const SceneLayout& layout) {
  terrainTiles = LoadTiles(TILE_TYPE_TERRAIN, layout.terrainLayout);
  crateTiles = LoadTiles(TILE_TYPE_CRATE, layout.cratesLayout);
  grassTiles = LoadTiles(TILE_TYPE_GRASS, layout.grassLayout);
}

std::vector<Tile*> SceneTiles::LoadTiles(TileType type, const Utils::Layout& layout) {
  std::vector<Tile*> tiles;
  for(size_t i = 0; i < layout.size(); ++i) {
    for(size_t j = 0; j < layout[i].size(); ++j) {
      int value = layout[i][j];
      if(value != -1) {
        tiles.push_back(TileFactory::createTile(type, {(float) j, (float) i}, value));
      }
    }
  }
  return tiles;
}

void SceneTiles::UpdateTiles(SDL_State &state, float mapHeight, float cameraX) {
  float mapOffsetY = state.windowHeight - mapHeight;
  if(mapOffsetY < 0) mapOffsetY = 0; // hvis vinduet er mindre end map

  for(auto& tile : terrainTiles) {
    tile->dstRect.y = tile->position.y * TILE_SIZE + mapOffsetY + tile->offset.y;
    tile->update({cameraX, 0.0f});
  }
  for(auto& tile : crateTiles) {
    tile->dstRect.y = tile->position.y * TILE_SIZE + mapOffsetY + tile->offset.y;
    tile->update({cameraX, 0.0f});
  }
  for(auto& tile : grassTiles) {
    tile->dstRect.y = tile->position.y * TILE_SIZE + mapOffsetY + tile->offset.y;
    tile->update({cameraX, 0.0f});
  }
}

void SceneTiles::DrawTiles(SDL_Renderer* renderer) const {
  for(const auto& tile : terrainTiles) {
    tile->draw(renderer);
  }

  for(const auto& tile : crateTiles) {
    tile->draw(renderer);
  }

  for(const auto& tile : grassTiles) {
    tile->draw(renderer);
  }
}

Scene::Scene(unsigned int level, const std::string& name)
  : level(level)
  , name(name)
  , layout(SceneLayout(level))
  , tiles(SceneTiles(layout))
{
  Log::Info("Indlæste scene \"{}\" successfuldt", name);
}

void Scene::update(SDL_State& state) noexcept {
  bg.update(state);

  cameraX = 0.0f;
  if(state.keyState[SDL_SCANCODE_RIGHT] || state.keyState[SDL_SCANCODE_D]) cameraX += scrollSpeed * state.deltaTime;
  if(state.keyState[SDL_SCANCODE_LEFT] || state.keyState[SDL_SCANCODE_A]) cameraX -= scrollSpeed * state.deltaTime;

  float mapHeight = layout.terrainLayout.size() * TILE_SIZE;
  tiles.UpdateTiles(state, mapHeight, cameraX);
};

void Scene::draw(SDL_Renderer* renderer) const noexcept {
  bg.render(renderer);

  tiles.DrawTiles(renderer);
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
