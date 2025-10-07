#include "Scene.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "core/ResourceManager.hpp"
#include "core/TileManager.hpp"

namespace Scene {

  void UpdateTileGroup(TileGroup& group, float mapOffsetY, float cameraX) {
    for(auto& tile : group) {
      tile->dstRect.y = tile->position.y * TILE_SIZE + mapOffsetY + tile->offset.y;
      tile->update({cameraX, 0.0f});
    }
  }

  void DrawTileGroup(const TileGroup& group, SDL_Renderer* renderer) {
    for(auto& tile : group) {
      tile->draw(renderer);
    }
  }

Layout::Layout(unsigned int level) {
  bgPalmsLayout     = LoadLevelLayout(level, "bg_palms");
  coinsLayout       = LoadLevelLayout(level, "coins");
  constraintsLayout = LoadLevelLayout(level, "constraints");
  cratesLayout      = LoadLevelLayout(level, "crates");
  enemiesLayout     = LoadLevelLayout(level, "enemies");
  fgPalmsLayout     = LoadLevelLayout(level, "fg_palms");
  grassLayout       = LoadLevelLayout(level, "grass");
  playerSetupLayout = LoadLevelLayout(level, "player");
  terrainLayout     = LoadLevelLayout(level, "terrain");
  constraintLayout  = LoadLevelLayout(level, "constraints");
}

Utils::TileLayer Layout::LoadLevelLayout(unsigned int level, const std::string& name) {
  Utils::TileLayer result = Utils::LoadCSVFile(std::format("levels/{0}/level_{0}_{1}.csv", level, name));
  if(result.empty()) {
    Log::Error("Kunne ikke validere resultat fra Utils::LoadCSVFile ({})", name);
    return {};
  }

  return result;
}

Tiles::Tiles(const Layout& layout) {
  terrainTiles     = LoadTiles(TILE_TYPE_TERRAIN, layout.terrainLayout);
  crateTiles       = LoadTiles(TILE_TYPE_CRATE, layout.cratesLayout);
  grassTiles       = LoadTiles(TILE_TYPE_GRASS, layout.grassLayout);
  playerSetupTiles = LoadTiles(TILE_TYPE_PLAYER_SETUP, layout.playerSetupLayout);
  enemyTiles       = LoadTiles(TILE_TYPE_ENEMY, layout.enemiesLayout);
  coinsTiles       = LoadTiles(TILE_TYPE_COIN, layout.coinsLayout);
  fgPalmsTiles     = LoadTiles(TILE_TYPE_FG_PALM, layout.fgPalmsLayout);
  bgPalmsTiles     = LoadTiles(TILE_TYPE_BG_PALM, layout.bgPalmsLayout);
  constraintTiles  = LoadTiles(TILE_TYPE_CONSTRAINT, layout.constraintLayout);
}

TileGroup Tiles::LoadTiles(TileType type, const Utils::TileLayer& layout) {
  TileGroup tiles;
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

void Tiles::UpdateTiles(SDL_State &state, float mapHeight, float cameraX) {
  float mapOffsetY = state.windowHeight - mapHeight;
  if(mapOffsetY < 0) mapOffsetY = 0; // hvis vinduet er mindre end map

  for(auto* group : allGroups) {
    UpdateTileGroup(*group, mapOffsetY, cameraX);
  }

}

void Tiles::DrawTiles(SDL_Renderer* renderer) const {
  for(const auto* group : allGroups) {
    DrawTileGroup(*group, renderer);
  }
}

Manager::Manager(unsigned int level, const std::string& name)
  : level(level)
  , name(name)
  , layout(Layout(level))
  , tiles(Tiles(layout))
{
  Log::Info("Indlæste scene \"{}\" successfuldt", name);
}

void Manager::update(SDL_State& state) noexcept {
  bg.update(state);

  state.cameraX = 0.0f;
  state.cameraY = 0.0f;
  if(state.keyState[SDL_SCANCODE_RIGHT] || state.keyState[SDL_SCANCODE_D]) state.cameraX += scrollSpeed * state.deltaTime;
  if(state.keyState[SDL_SCANCODE_LEFT] || state.keyState[SDL_SCANCODE_A]) state.cameraX -= scrollSpeed * state.deltaTime;

  float mapHeight = layout.terrainLayout.size() * TILE_SIZE;
  tiles.UpdateTiles(state, mapHeight, state.cameraX);
};

void Manager::draw(SDL_Renderer* renderer) const noexcept {
  bg.render(renderer);

  tiles.DrawTiles(renderer);
};


void Manager::saveScene(const std::filesystem::path& path) {};

};
