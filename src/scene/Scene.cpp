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

static void WriteCSV(const std::filesystem::path& filePath, const std::vector<std::vector<int>>& data) {
  std::filesystem::create_directories(filePath.parent_path());
  std::ofstream out(filePath);
  if (!out.is_open()) {
    Log::Error("Kunne ikke åbne fil til skrivning: {}", filePath.string());
    return;
  }

  for (size_t y = 0; y < data.size(); ++y) {
    for (size_t x = 0; x < data[y].size(); ++x) {
      out << data[y][x];
      if (x < data[y].size() - 1) out << ",";
    }
    out << "\n";
  }
  out.close();
  Log::Info("Gemte {}", filePath.string());
}

// Bestem max grid size på alle tiletypes
static Vec2<int> ComputeMapSize(const Tiles& tiles) {
  int maxX = 0, maxY = 0;
  for (auto* group : tiles.allGroups) {
    for (auto* tile : *group) {
      maxX = std::max(maxX, static_cast<int>(tile->position.x));
      maxY = std::max(maxY, static_cast<int>(tile->position.y));
    }
  }
  // +1 fordi grid-koordinater er 0-indexed
  return { maxX + 1, maxY + 1 };
}

// Konverter tilegroup til 2D matrix
static std::vector<std::vector<int>> MakeCSVDataForType(const TileGroup& group, int width, int height) {
  std::vector<std::vector<int>> grid(height, std::vector<int>(width, -1));
  for (auto* tile : group) {
    int x = static_cast<int>(tile->position.x);
    int y = static_cast<int>(tile->position.y);
    if (x >= 0 && x < width && y >= 0 && y < height) {
      grid[y][x] = tile->getTileIndex();
    }
  }
  return grid;
}

void Manager::saveScene(const std::filesystem::path& dirPath) {
    Log::Info("Gemmer scene til: {}", dirPath.string());

    // Tving fast størrelse 60x11
    const int width  = 60;
    const int height = 11;

    Log::Info("Map size (fast): {} x {}", width, height);

    struct TypeFile {
        TileType type;
        const char* name;
        TileGroup* group;
    } typeFiles[] = {
        { TILE_TYPE_BG_PALM,     "level_0_bg_palms.csv",   &tiles.bgPalmsTiles },
        { TILE_TYPE_COIN,        "level_0_coins.csv",      &tiles.coinsTiles },
        { TILE_TYPE_CONSTRAINT,  "level_0_constraints.csv",&tiles.constraintTiles },
        { TILE_TYPE_CRATE,       "level_0_crates.csv",     &tiles.crateTiles },
        { TILE_TYPE_ENEMY,       "level_0_enemies.csv",    &tiles.enemyTiles },
        { TILE_TYPE_FG_PALM,     "level_0_fg_palms.csv",   &tiles.fgPalmsTiles },
        { TILE_TYPE_GRASS,       "level_0_grass.csv",      &tiles.grassTiles },
        { TILE_TYPE_PLAYER_SETUP,"level_0_player.csv",     &tiles.playerSetupTiles },
        { TILE_TYPE_TERRAIN,     "level_0_terrain.csv",    &tiles.terrainTiles }
    };

    for (const auto& entry : typeFiles) {
        auto data = MakeCSVDataForType(*entry.group, width, height);
        WriteCSV(dirPath / entry.name, data);
    }

    Log::Info("Scene gemt til: {}", dirPath.string());
}

// bitmask: N=1, E=2, S=4, W=8
const std::array<int,16> Tiles::TERRAIN_16_MAP = {
  /*0000*/ 15, // isoleret (ingen naboer)
  /*0001*/  9, // N -> bottom edge
  /*0010*/  4, // E -> left edge
  /*0011*/  8, // N+E -> bottom-left corner
  /*0100*/  1, // S -> top edge
  /*0101*/  5, // N+S -> vertical (brug center)
  /*0110*/  0, // E+S -> top-left corner
  /*0111*/  4, // N+E+S -> left side
  /*1000*/  3, // W -> right edge
  /*1001*/ 10, // N+W -> bottom-right corner
  /*1010*/ 13, // E+W -> horizontal mid (platform mid)
  /*1011*/  9, // N+E+W -> bottom edge (T-junction)
  /*1100*/  2, // S+W -> top-right corner
  /*1101*/  6, // N+S+W -> right side
  /*1110*/  1, // E+S+W -> top edge
  /*1111*/  5  // omgivet -> center
};


Tile* Tiles::GetTileOfType(int gx, int gy, TileType type) {
  long long key = makeTileKey(gx, gy);
  auto it = tileLookup.find(key);
  if (it == tileLookup.end()) return nullptr;
  for (auto* t : it->second) {
    if (t && t->getType() == type) return t;
  }
  return nullptr;
}

bool Tiles::HasTileOfType(int gx, int gy, TileType type) {
  return GetTileOfType(gx, gy, type) != nullptr;
}

int Tiles::Make4BitMask(int x, int y, std::function<bool(int,int)> isSame) {
  int mask = 0;
  if (isSame(x, y-1)) mask |= 1;  // N
  if (isSame(x+1, y)) mask |= 2;  // E
  if (isSame(x, y+1)) mask |= 4;  // S
  if (isSame(x-1, y)) mask |= 8;  // W
  return mask;
}

void Tiles::AutotileRecalcAt(int x, int y) {
  Tile* t = GetTileOfType(x, y, TILE_TYPE_TERRAIN);
  if (!t) return;

  auto isSame = [&](int ax, int ay) {
      return HasTileOfType(ax, ay, TILE_TYPE_TERRAIN);
  };

  int mask = Make4BitMask(x, y, isSame);
  int tileIndex = TERRAIN_16_MAP[mask];

  // --- vandret platform ---
  if (!isSame(x, y-1) && !isSame(x, y+1)) {
      bool left  = isSame(x-1, y);
      bool right = isSame(x+1, y);
      if (!left && right)      tileIndex = 12; // venstre cap
      else if (left && right)  tileIndex = 13; // mid
      else if (left && !right) tileIndex = 14; // højre cap
      else                     tileIndex = 15; // isoleret
  }

  // --- lodret søjle ---
  if (!isSame(x-1, y) && !isSame(x+1, y)) {
      bool up   = isSame(x, y-1);
      bool down = isSame(x, y+1);
      if (!up && down)        tileIndex = 3;  // top cap (bund under sig)
      else if (up && !down)   tileIndex = 11;  // bottom cap (top over sig)
      else if (up && down)    tileIndex = 7;  // mid (begge naboer)
      else                    tileIndex = 15; // helt isoleret
  }

  t->setTileIndex(tileIndex);
}

void Tiles::AutotileRecalcNeighborsAround(int x, int y) {
  AutotileRecalcAt(x, y);
  AutotileRecalcAt(x, y-1);
  AutotileRecalcAt(x+1, y);
  AutotileRecalcAt(x, y+1);
  AutotileRecalcAt(x-1, y);
}

void Tiles::AutotileAllTerrain() {
  for (auto* t : terrainTiles) {
    int gx = static_cast<int>(t->position.x);
    int gy = static_cast<int>(t->position.y);
    AutotileRecalcAt(gx, gy);
  }
}

void UpdateTileGroup(TileGroup& group, float mapOffsetY, float cameraX) {
  for(auto& tile : group) {
    tile->update({cameraX, 0.0f});
    tile->dstRect.y += mapOffsetY;
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

  AutotileAllTerrain();
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


  if (tile->getType() == TILE_TYPE_TERRAIN) {
    tiles.AutotileRecalcNeighborsAround(gx, gy);
  }
}


void Manager::removeTileAt(int gridX, int gridY, int layerIndex) {
  // Husk om der var terrain her, så vi ved om vi skal autotile naboer bagefter
  Tile* before = tiles.GetTileOfType(gridX, gridY, TILE_TYPE_TERRAIN);
  tiles.RemoveTile(gridX, gridY, layerIndex);
  if (before) {
    tiles.AutotileRecalcNeighborsAround(gridX, gridY);
  }
}

void Manager::removeLayerTiles(int gridX, int gridY, int layerIndex) {
  Tile* before = tiles.GetTileOfType(gridX, gridY, TILE_TYPE_TERRAIN);
  tiles.RemoveTile(gridX, gridY, layerIndex);
  if (before) {
    tiles.AutotileRecalcNeighborsAround(gridX, gridY);
  }
}

Tile* Manager::getTileAt(int gridX, int gridY) {
  return tiles.GetTile(gridX, gridY);
}

};
