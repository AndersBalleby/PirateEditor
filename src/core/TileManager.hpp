#pragma once

#include "math/vec.hpp"
#include <vector>
#include <SDL3/SDL.h>
enum TileType {
  TILE_TYPE_TERRAIN,
  TILE_TYPE_COIN,
  TILE_TYPE_GRASS,
  TILE_TYPE_CRATE,
  TILE_TYPE_PALM_BG,
  TILE_TYPE_PALM_FG_LARGE,
  TILE_TYPE_PALM_FG_SMALL,
};

class Tile {
public:
  Tile(TileType type);
  ~Tile();
private:
  TileType type;
  SDL_Texture* texture;
  Vec2<float> position;
};

class TileManager {
public:
    TileManager();
    ~TileManager();
private:
  std::vector<Tile*> tiles;
  bool init();
};
