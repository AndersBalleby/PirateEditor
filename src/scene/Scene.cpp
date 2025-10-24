#include "Scene.hpp"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "core/ResourceManager.hpp"
#include "core/TileManager.hpp"
#include "logging/Logger.hpp"

namespace Scene {

void UpdateTileGroup(TileGroup& group, float mapOffsetY, float cameraX) {
  for(auto& tile : group) {
    tile->update({cameraX, 0.0f});
    tile->dstRect.y += mapOffsetY; // apply mapOffsetY once after updating
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
  terrainTiles     = LoadTiles(TILE_TYPE_TERRAIN, layout.terrainLayout, tileLookup);
  crateTiles       = LoadTiles(TILE_TYPE_CRATE, layout.cratesLayout, tileLookup);
  grassTiles       = LoadTiles(TILE_TYPE_GRASS, layout.grassLayout, tileLookup);
  playerSetupTiles = LoadTiles(TILE_TYPE_PLAYER_SETUP, layout.playerSetupLayout, tileLookup);
  enemyTiles       = LoadTiles(TILE_TYPE_ENEMY, layout.enemiesLayout, tileLookup);
  coinsTiles       = LoadTiles(TILE_TYPE_COIN, layout.coinsLayout, tileLookup);
  fgPalmsTiles     = LoadTiles(TILE_TYPE_FG_PALM, layout.fgPalmsLayout, tileLookup);
  bgPalmsTiles     = LoadTiles(TILE_TYPE_BG_PALM, layout.bgPalmsLayout, tileLookup);
  constraintTiles  = LoadTiles(TILE_TYPE_CONSTRAINT, layout.constraintLayout, tileLookup);
}

inline long long Tiles::makeTileKey(int x, int y) {
  return (static_cast<long long>(x) << 32) | static_cast<unsigned int>(y);
}

Tile* Tiles::GetTile(int gridX, int gridY) {
  long long key = makeTileKey(gridX, gridY);
  auto it = tileLookup.find(key);
  if(it != tileLookup.end()) {
    return it->second.front();
  }
  return nullptr;
}

TileGroup Tiles::LoadTiles(TileType type, const Utils::TileLayer& layout, std::unordered_map<long long, std::vector<Tile*>>& lookup) {
  TileGroup tiles;
  for(size_t i = 0; i < layout.size(); ++i) {
    for(size_t j = 0; j < layout[i].size(); ++j) {
      int value = layout[i][j];
      if(value != -1) {
        Tile* tile = TileFactory::createTile(type, {(float) j, (float) i}, value);
        tiles.push_back(tile);

        lookup[makeTileKey((int)j, (int)i)].push_back(tile);
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

void Tiles::RemoveTile(int gridX, int gridY, int layerIndex) {
  if (layerIndex < 0 || layerIndex >= (int)layerGroups.size())
    return;

  long long key = makeTileKey(gridX, gridY);
  auto it = tileLookup.find(key);
  if (it == tileLookup.end()) return;

  auto& tilesAtPos = it->second;
  if (tilesAtPos.empty()) return;

  auto& targetGroups = layerGroups[layerIndex];

  // Alle tiles i layerIndex der matcher positionen
  std::vector<Tile*> toRemove;
  for (auto* group : targetGroups) {
      auto& g = *group;
      for (auto* tile : g) {
          if ((int)tile->position.x == gridX && (int)tile->position.y == gridY) {
              toRemove.push_back(tile);
          }
      }
  }

  // Fjern fra groups
  for (auto* group : targetGroups) {
      auto& g = *group;
      g.erase(std::remove_if(g.begin(), g.end(), [&](Tile* t) {
          return std::find(toRemove.begin(), toRemove.end(), t) != toRemove.end();
      }), g.end());
  }

  // Fjern fra lookup
  tilesAtPos.erase(std::remove_if(tilesAtPos.begin(), tilesAtPos.end(),
    [&](Tile* t) {
      return std::find(toRemove.begin(), toRemove.end(), t) != toRemove.end();
    }),
    tilesAtPos.end());

  if (tilesAtPos.empty())
    tileLookup.erase(it);

  for (auto* t : toRemove)
    delete t;
}



void Tiles::DrawTiles(SDL_Renderer* renderer, int visibleLayer) const {
  const Uint8 activeAlpha = 255;
  const Uint8 inactiveAlpha = 80;

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  // --- NORMAL MODE ---
  if (visibleLayer == -1) {
    // Tegn alle tiles normalt, uden gennemsigtighed
    for (const auto* group : allGroups) {
      for (auto* tile : *group) {
        if (!tile->texture) continue;
          tile->draw(renderer);
      }
    }
    return;
  }

  // --- LAYER MODE ---
  for (int i = 0; i < (int)layerGroups.size(); ++i) {
    Uint8 layerAlpha = (i == visibleLayer) ? activeAlpha : inactiveAlpha;

    for (auto* group : layerGroups[i]) {
      for (auto* tile : *group) {
        if (!tile->texture) continue;

        Uint8 oldAlpha = 255;
        SDL_GetTextureAlphaMod(tile->texture, &oldAlpha);
        SDL_SetTextureAlphaMod(tile->texture, layerAlpha);

        tile->draw(renderer);

        SDL_SetTextureAlphaMod(tile->texture, oldAlpha);
      }
    }
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
  if((state.keyState[SDL_SCANCODE_RIGHT] || state.keyState[SDL_SCANCODE_D]) && state.cameraPos.x < 3072) {
    state.cameraX += scrollSpeed * state.deltaTime;
  }

  if((state.keyState[SDL_SCANCODE_LEFT] || state.keyState[SDL_SCANCODE_A]) && state.cameraPos.x > -512) {
    state.cameraX -= scrollSpeed * state.deltaTime;
  }

  float mapHeight = layout.terrainLayout.size() * TILE_SIZE;
  tiles.UpdateTiles(state, mapHeight, state.cameraPos.x);
};

void Manager::draw(SDL_Renderer* renderer, int visibleLayer) const noexcept {
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  bg.render(renderer);
  tiles.DrawTiles(renderer, visibleLayer);
};

void Manager::addTileToLayer(Tile* tile, int layerIndex) {
    if (!tile) return;
    if (layerIndex < 0 || layerIndex >= (int)tiles.layerGroups.size()) return;

    switch (tile->getType()) {
        case TILE_TYPE_TERRAIN:       tiles.terrainTiles.push_back(tile); break;
        case TILE_TYPE_CRATE:         tiles.crateTiles.push_back(tile); break;
        case TILE_TYPE_GRASS:         tiles.grassTiles.push_back(tile); break;
        case TILE_TYPE_PLAYER_SETUP:  tiles.playerSetupTiles.push_back(tile); break;
        case TILE_TYPE_ENEMY:         tiles.enemyTiles.push_back(tile); break;
        case TILE_TYPE_COIN:          tiles.coinsTiles.push_back(tile); break;
        case TILE_TYPE_FG_PALM:       tiles.fgPalmsTiles.push_back(tile); break;
        case TILE_TYPE_BG_PALM:       tiles.bgPalmsTiles.push_back(tile); break;
        case TILE_TYPE_CONSTRAINT:    tiles.constraintTiles.push_back(tile); break;
        default: break;
    }

    // Beregn GRID koordinater
    const int gx = static_cast<int>(tile->position.x);
    const int gy = static_cast<int>(tile->position.y);
    long long key = Tiles::makeTileKey(gx, gy);
    tiles.tileLookup[key].push_back(tile);
}


void Manager::removeTileAt(int gridX, int gridY, int layerIndex) {
  tiles.RemoveTile(gridX, gridY, layerIndex);
}

void Manager::removeLayerTiles(int gridX, int gridY, int layerIndex) {
  tiles.RemoveTile(gridX, gridY, layerIndex);
}

Tile* Manager::getTileAt(int gridX, int gridY) {
  return tiles.GetTile(gridX, gridY);
}

void Manager::saveScene(const std::filesystem::path& path) {};

};
