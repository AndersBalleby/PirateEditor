#pragma once

#include "math/vec.hpp"
#include "ResourceManager.hpp"
#include <vector>
#include <SDL3/SDL.h>

/*
 TILE_TYPE_TERRAIN,
 TILE_TYPE_COIN,
 TILE_TYPE_GRASS,
 TILE_TYPE_CRATE,
 TILE_TYPE_PALM_BG,
 TILE_TYPE_PALM_FG_LARGE,
 TILE_TYPE_PALM_FG_SMALL,
 */

enum TileType {
  TILE_TYPE_TERRAIN,
  TILE_TYPE_CRATE,
};

class Tile {
public:
  Tile(TileType type, Vec2<float> position, int tileIndex = 0); // id er til tilemaps, default = 0
  ~Tile() = default;

  void update(float offsetX);
  void draw(SDL_Renderer* renderer) const;
private:

  void initializeFromTilemap(SDL_Texture* tileMapTex, Vec2<float> position, int tileIndex);
  void initializeStaticTile(SDL_Texture* texture, Vec2<float> position, Vec2<float> offset);

  TileType type;
  SDL_Texture* texture;
  Vec2<float> position;
  SDL_FRect dstRect;
  SDL_FRect srcRect;
  bool staticTile;
};

class TileFactory {
public:
    TileFactory();
    ~TileFactory() = default;

    static Tile* createTile(TileType type, Vec2<float> position, int tileIndex = 0);
private:
    std::vector<SDL_Texture*> textures;
};
