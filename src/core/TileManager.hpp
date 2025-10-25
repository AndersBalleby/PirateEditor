#pragma once

#include "math/vec.hpp"
#include "ResourceManager.hpp"
#include <vector>
#include <SDL3/SDL.h>

enum TileType {
  TILE_TYPE_TERRAIN,
  TILE_TYPE_CRATE,
  TILE_TYPE_GRASS,
  TILE_TYPE_PLAYER_SETUP,
  TILE_TYPE_ENEMY,
  TILE_TYPE_COIN,
  TILE_TYPE_FG_PALM, // Denne bruges udelukkende til LoadTiles (id)
  TILE_TYPE_BG_PALM,
  TILE_TYPE_FG_PALM_LARGE,
  TILE_TYPE_FG_PALM_SMALL,
  TILE_TYPE_CONSTRAINT,
};

class Tile {
public:
  Tile(TileType type, Vec2<float> position, int tileIndex = 0, bool inserted = false); // id er til tilemaps, default = 0
  ~Tile() = default;

  void update(Vec2<float> offset);
  void draw(SDL_Renderer* renderer) const;

  void setTileIndex(int tileIndex);
  int getTileIndex() const;

  TileType getType() const;

  SDL_FRect dstRect;
  SDL_Texture* texture;
  Vec2<float> position;
  Vec2<float> offset;
private:

  void initializeFromTilemap(SDL_Texture* tileMapTex, Vec2<float> position, int tileIndex, bool inserted);
  void initializeStaticTile(SDL_Texture* texture, Vec2<float> position, Vec2<float> offset, bool inserted);

  TileType type;
  SDL_FRect srcRect;
  bool staticTile;
  int currentTileIndex = 0;
};

class TileFactory {
public:
    TileFactory();
    ~TileFactory() = default;

    static Tile* createTile(TileType type, Vec2<float> position, int tileIndex = 0, bool inserted = false);
private:
    std::vector<SDL_Texture*> textures;
};
