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

void Tiles::RemoveTile(int gridX, int gridY, bool allTiles) {
    long long key = makeTileKey(gridX, gridY);
    auto it = tileLookup.find(key);
    if (it == tileLookup.end()) return;

    auto& tilesAtPos = it->second;
    if (tilesAtPos.empty()) return;

    if (allTiles) {
        // Fjern alle tiles på denne position fra ALLE grupper
        for (Tile* tile : tilesAtPos) {
            for (auto* group : allGroups) {
                auto& g = *group;
                g.erase(std::remove(g.begin(), g.end(), tile), g.end());
            }
        }

        // Fjern entry helt fra lookup-tabellen
        tileLookup.erase(it);
    }
    else {
        // Kun fjern den øverste tile som før
        Tile* target = tilesAtPos.back();
        tilesAtPos.pop_back();

        for (auto* group : allGroups) {
            auto& g = *group;
            g.erase(std::remove(g.begin(), g.end(), target), g.end());
        }

        if (tilesAtPos.empty()) {
            tileLookup.erase(it);
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
  tiles.UpdateTiles(state, mapHeight, state.cameraX);
};

void Manager::draw(SDL_Renderer* renderer) const noexcept {
  bg.render(renderer);

  tiles.DrawTiles(renderer);
};

void Manager::removeTileAt(int gridX, int gridY) {
  tiles.RemoveTile(gridX, gridY);
}

void Manager::removeLayerTiles(int gridX, int gridY) {
  tiles.RemoveTile(gridX, gridY, true);
}

void Manager::saveScene(const std::filesystem::path& path) {};

};
