#include "Scene.hpp"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "logging/Logger.hpp"
#include "resources/ResourceManager.hpp"
#include <filesystem>

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

static void RenderMiniBackground(SDL_Renderer *renderer, int mapW, int mapH) {
  SDL_Texture* sky_top    = ResourceManager::loadTexture("resources/decoration/sky/sky_top.png");
  SDL_Texture* sky_middle = ResourceManager::loadTexture("resources/decoration/sky/sky_middle.png");
  SDL_Texture* sky_bottom = ResourceManager::loadTexture("resources/decoration/sky/sky_bottom.png");

  if(!sky_top || !sky_middle || !sky_bottom) {
    Log::Error("Kunne ikke indlæse sky-textures til thumbnail");
    return;
  }

  float topH    = mapH * 0.25f;
  float middleH = mapH * 0.5f;
  float bottomH = mapH * 0.25f;

  SDL_FRect topRect    { 0, 0, (float)mapW, topH };
  SDL_FRect middleRect { 0, topH, (float)mapW, middleH };
  SDL_FRect bottomRect { 0, topH + middleH, (float)mapW, bottomH };

  // Tegn lagene
  SDL_RenderTexture(renderer, sky_top,    nullptr, &topRect);
  SDL_RenderTexture(renderer, sky_middle, nullptr, &middleRect);
  SDL_RenderTexture(renderer, sky_bottom, nullptr, &bottomRect);

  // Tegn et par “faste” skyer for at gøre previewet levende
  SDL_Texture* cloud1 = ResourceManager::loadTexture("resources/decoration/clouds/1.png");
  SDL_Texture* cloud2 = ResourceManager::loadTexture("resources/decoration/clouds/2.png");
  SDL_Texture* cloud3 = ResourceManager::loadTexture("resources/decoration/clouds/3.png");

  if (cloud1 && cloud2 && cloud3) {
    float cw, ch;
    SDL_GetTextureSize(cloud1, &cw, &ch);
    SDL_FRect c1 { mapW * 0.15f, mapH * 0.10f, cw, ch };
    SDL_FRect c2 { mapW * 0.55f, mapH * 0.18f, cw, ch };
    SDL_GetTextureSize(cloud2, &cw, &ch);
    SDL_FRect c3 { mapW * 0.35f, mapH * 0.05f, cw, ch };
    SDL_GetTextureSize(cloud3, &cw, &ch);
    SDL_RenderTexture(renderer, cloud1, nullptr, &c1);
    SDL_RenderTexture(renderer, cloud2, nullptr, &c2);
    SDL_RenderTexture(renderer, cloud3, nullptr, &c3);
  }
}

SDL_Texture* BuildSceneThumbnail(SDL_Renderer* renderer, const std::string& sceneName, int thumbW, int thumbH) {
  Layout lay(sceneName);
  Tiles temp(lay);
  temp.AutotileAllTerrain();

  for (auto* group : temp.allGroups)
    for (auto* t : *group)
      t->update({0.f, 0.f}); // nulstil world offse

  Vec2<int> grid = ComputeMapSize(temp);
  int mapW = std::max(1, grid.x * TILE_SIZE);
  int mapH = std::max(1, grid.y * TILE_SIZE);

  const int MAX_RT = 4096;
  if(mapW > MAX_RT || mapH > MAX_RT) {
    float s = std::min((float) MAX_RT / mapW, (float) MAX_RT / mapH);
    mapW = std::max(1, (int)(mapW * s));
    mapH = std::max(1, (int)(mapH * s));
  }

  SDL_Texture* fullTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mapW, mapH);
  if(!fullTex) return nullptr;
  SDL_SetTextureBlendMode(fullTex, SDL_BLENDMODE_BLEND);

  SDL_SetRenderTarget(renderer, fullTex);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  SDL_SetRenderDrawColor(renderer, 18, 18, 24, 255);
  SDL_RenderClear(renderer);

  RenderMiniBackground(renderer, mapW, mapH);

  temp.DrawTiles(renderer);

  SDL_SetRenderTarget(renderer, nullptr);

  SDL_Texture* thumb = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, thumbW, thumbH);
  if(!thumb) {
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_DestroyTexture(fullTex);
    return nullptr;
  }

  SDL_SetTextureBlendMode(thumb, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(renderer, thumb);
  SDL_SetRenderDrawColor(renderer, 12, 12, 16, 255);
  SDL_RenderClear(renderer);

  // Vi vil kun vise en del af mappet – fx 1/5
  float visibleFraction = 0.2f; // 20% af map-bredden
  int viewW = (int)(mapW * visibleFraction);
  int viewH = mapH;

  // Vi tager midten af mappet (eller brug 0 for venstre side)
  int viewX = 0;
  int viewY = 0;

  // Beregn forhold mellem scene og thumbnail
  float sx = (float) thumbW / viewW;
  float sy = (float) thumbH / viewH;

  // Brug max i stedet for min => zoom mere ind, så vi fylder hele feltet
  float s = std::max(sx, sy);

  // Beregn hvor meget vi skal beskære fra source (udenfor synsfeltet)
  float cropW = thumbW / s;
  float cropH = thumbH / s;

  // Center cropping
  float cropX = viewX + (viewW - cropW) / 2.f;
  float cropY = viewY + (viewH - cropH) / 2.f;

  // Source rectangle (hvad vi faktisk viser)
  SDL_FRect src {
    cropX,
    cropY,
    cropW,
    cropH
  };

  // Destination udfylder hele thumbnail
  SDL_FRect dst {
    0.f, 0.f,
    (float)thumbW,
    (float)thumbH
  };

  // Tegn med crop zoom
  SDL_RenderTexture(renderer, fullTex, &src, &dst);


  // Tegn kun det udsnit vi valgte
  SDL_RenderTexture(renderer, fullTex, &src, &dst);

  SDL_SetRenderTarget(renderer, nullptr);
  SDL_DestroyTexture(fullTex);

  return thumb;
}

void Manager::saveScene(const std::string& sceneName) {
  std::filesystem::path sceneDir = std::filesystem::path("scenes") / sceneName;
  std::filesystem::create_directories(sceneDir);

  Log::Info("Gemmer scene til: {}", sceneDir.string());

  const int width  = 60;
  const int height = 11;

  Log::Info("Map size (fast): {} x {}", width, height);

  struct TypeFile {
    TileType type;
    const char* suffix;
    TileGroup* group;
  } typeFiles[] = {
    { TILE_TYPE_BG_PALM,     "_bg_palms.csv",   &tiles.bgPalmsTiles },
    { TILE_TYPE_COIN,        "_coins.csv",      &tiles.coinsTiles },
    { TILE_TYPE_CONSTRAINT,  "_constraints.csv",&tiles.constraintTiles },
    { TILE_TYPE_CRATE,       "_crates.csv",     &tiles.crateTiles },
    { TILE_TYPE_ENEMY,       "_enemies.csv",    &tiles.enemyTiles },
    { TILE_TYPE_FG_PALM,     "_fg_palms.csv",   &tiles.fgPalmsTiles },
    { TILE_TYPE_GRASS,       "_grass.csv",      &tiles.grassTiles },
    { TILE_TYPE_PLAYER_SETUP,"_player.csv",     &tiles.playerSetupTiles },
    { TILE_TYPE_TERRAIN,     "_terrain.csv",    &tiles.terrainTiles }
  };

  for (const auto& entry : typeFiles) {
    auto data = MakeCSVDataForType(*entry.group, width, height);
    std::filesystem::path filePath = sceneDir / (sceneName + entry.suffix);
    WriteCSV(filePath, data);
    Log::Info("Gemte {}", filePath.string());
  }

  Log::Info("Scene gemt til: {}", sceneName);
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

Tiles::Tiles(Tiles&& other) noexcept
  : terrainTiles(std::move(other.terrainTiles))
  , crateTiles(std::move(other.crateTiles))
  , grassTiles(std::move(other.grassTiles))
  , playerSetupTiles(std::move(other.playerSetupTiles))
  , enemyTiles(std::move(other.enemyTiles))
  , coinsTiles(std::move(other.coinsTiles))
  , fgPalmsTiles(std::move(other.fgPalmsTiles))
  , bgPalmsTiles(std::move(other.bgPalmsTiles))
  , constraintTiles(std::move(other.constraintTiles))
  , tileLookup(std::move(other.tileLookup))
{
  rebuildPointers_();
}

Tiles& Tiles::operator=(Tiles&& other) noexcept {
  if (this != &other) {
    terrainTiles      = std::move(other.terrainTiles);
    crateTiles        = std::move(other.crateTiles);
    grassTiles        = std::move(other.grassTiles);
    playerSetupTiles  = std::move(other.playerSetupTiles);
    enemyTiles        = std::move(other.enemyTiles);
    coinsTiles        = std::move(other.coinsTiles);
    fgPalmsTiles      = std::move(other.fgPalmsTiles);
    bgPalmsTiles      = std::move(other.bgPalmsTiles);
    constraintTiles   = std::move(other.constraintTiles);
    tileLookup        = std::move(other.tileLookup);
    rebuildPointers_();
  }
  return *this;
}

void Tiles::rebuildPointers_() {
  allGroups = {
    &bgPalmsTiles,
    &terrainTiles,
    &grassTiles,
    &crateTiles,
    &enemyTiles,
    &fgPalmsTiles,
    &coinsTiles,
    &constraintTiles,
    &playerSetupTiles
  };

  layerGroups = {
    std::vector<TileGroup*>{ &bgPalmsTiles },
    std::vector<TileGroup*>{ &terrainTiles, &crateTiles, &grassTiles, &enemyTiles },
    std::vector<TileGroup*>{ &fgPalmsTiles, &coinsTiles, &playerSetupTiles, &constraintTiles }
  };
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

Utils::TileLayer Layout::LoadSceneLayout(const std::string &sceneName, const std::string &suffix) {
  std::filesystem::path p = std::filesystem::path("scenes") / sceneName / (sceneName + "_" + suffix + ".csv");
  Utils::TileLayer result = Utils::LoadCSVFile(p.string());
  if(result.empty()) {
    Log::Error("Kunne ikke indlæse scene fra CSV: {}", p.string());
    return {};
  }

  return result;
}

Layout::Layout(const std::string& sceneName) {
  bgPalmsLayout     = LoadSceneLayout(sceneName, "bg_palms");
  coinsLayout       = LoadSceneLayout(sceneName, "coins");
  constraintsLayout = LoadSceneLayout(sceneName, "constraints");
  cratesLayout      = LoadSceneLayout(sceneName, "crates");
  enemiesLayout     = LoadSceneLayout(sceneName, "enemies");
  fgPalmsLayout     = LoadSceneLayout(sceneName, "fg_palms");
  grassLayout       = LoadSceneLayout(sceneName, "grass");
  playerSetupLayout = LoadSceneLayout(sceneName, "player");
  terrainLayout     = LoadSceneLayout(sceneName, "terrain");
  constraintLayout  = LoadSceneLayout(sceneName, "constraints");
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

void Manager::update(SDL_State& state, bool lockCamera) noexcept {
  bg.update(state);

  state.cameraX = 0.0f;
  if(!lockCamera) {
    if((state.keyState[SDL_SCANCODE_RIGHT] || state.keyState[SDL_SCANCODE_D]) && state.cameraPos.x < 3072) {
      state.cameraX += scrollSpeed * state.deltaTime;
    }

    if((state.keyState[SDL_SCANCODE_LEFT] || state.keyState[SDL_SCANCODE_A]) && state.cameraPos.x > -512) {
      state.cameraX -= scrollSpeed * state.deltaTime;
    }
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

static void FreeTileGroup(TileGroup& g) {
  for(auto* t : g) delete t;
  g.clear();
}

static void FreeAllTiles(Tiles& t) {
  for(auto* group : t.allGroups) {
    FreeTileGroup(*group);
  }

  t.tileLookup.clear();
}

void Manager::loadSceneFromFolder(const std::string& sceneName) {
  Layout newLayout(sceneName);

  if(newLayout.terrainLayout.empty()) {
    Log::Warn("Scene '{}' er tom - initialiserer som en ny scene", sceneName);
  }

  FreeAllTiles(tiles);

  layout = std::move(newLayout);
  tiles = Tiles(layout);
  tiles.AutotileAllTerrain();

  name = sceneName;
  Log::Info("Scene '{}' indlæst fra 'scenes/{}'", name, sceneName);
}

Tile* Manager::getTileAt(int gridX, int gridY) {
  return tiles.GetTile(gridX, gridY);
}

};
