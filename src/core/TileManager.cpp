#include "TileManager.hpp"
#include "SDL3/SDL_render.h"

//
// TILE FACTORY
//
TileFactory::TileFactory() {}

Tile* TileFactory::createTile(TileType type, Vec2<float> position, int tileIndex) {
  Tile* tile = new Tile(type, position, tileIndex);

  return tile;
}

//
// TILES
//
Tile::Tile(TileType type, Vec2<float> position, int tileIndex)
    : type(type)
    , position(position)
    , texture(nullptr)
{
  switch (type) {
    case TILE_TYPE_TERRAIN:
      initializeFromTilemap(ResourceManager::loadTileMap("resources/terrain/terrain_tiles.png"), position, tileIndex);
      break;
    case TILE_TYPE_CRATE:
      initializeStaticTile(ResourceManager::loadTexture("resources/terrain/crate.png"), position, {0, 24});
      break;
    }
}

void Tile::update(float offsetX) {
  dstRect.x -= offsetX;
}

const float MAP_OFFSET_Y = 100.0f;
void Tile::draw(SDL_Renderer *renderer) const {
  if (!texture) {
      Log::Error("Kunne ikke tegne tile: passerede nullptr i texture");
      return;
    }

  if(staticTile) {
    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
  } else {
    SDL_RenderTexture(renderer, texture, &srcRect, &dstRect);
  }
}

void Tile::initializeStaticTile(SDL_Texture* texture, Vec2<float> position, Vec2<float> offset) {
  if(!texture) {
    Log::Error("Kunne ikke indlæse tile: passerede nullptr i tex");
    return;
  }

  float texW, texH;
  SDL_GetTextureSize(texture, &texW, &texH);

  dstRect.x = static_cast<float>(position.x * TILE_SIZE) + offset.x;
  dstRect.y = static_cast<float>(position.y * TILE_SIZE) + MAP_OFFSET_Y + offset.y;
  dstRect.w = texW;
  dstRect.h = texH;

  this->texture = texture;
  this->staticTile = true;
}

void Tile::initializeFromTilemap(SDL_Texture* tileMapTex, Vec2<float> position, int tileIndex) {
    if (!tileMapTex) {
        Log::Error("Kunne ikke indlæse tilemap: passerede nullptr i tileMapTex");
        return;
    }

    float tilesheetWidth = 0, tilesheetHeight = 0;
    SDL_GetTextureSize(tileMapTex, &tilesheetWidth, &tilesheetHeight);

    const int tilesPerRow = 4; // 256 / 64

    srcRect.x = static_cast<float>((tileIndex % tilesPerRow) * TILE_SIZE);
    srcRect.y = static_cast<float>((tileIndex / tilesPerRow) * TILE_SIZE);
    srcRect.w = TILE_SIZE;
    srcRect.h = TILE_SIZE;


    dstRect.x = static_cast<float>(position.x * TILE_SIZE);
    dstRect.y = static_cast<float>(position.y * TILE_SIZE) + MAP_OFFSET_Y;
    dstRect.w = TILE_SIZE;
    dstRect.h = TILE_SIZE;

    this->texture = tileMapTex;
    this->staticTile = false;
}
