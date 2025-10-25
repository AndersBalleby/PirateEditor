#include "TileManager.hpp"
#include "SDL3/SDL_render.h"

//
// TILE FACTORY
//
TileFactory::TileFactory() {}

Tile* TileFactory::createTile(TileType type, Vec2<float> position, int tileIndex, bool inserted) {
  Tile* tile = new Tile(type, position, tileIndex, inserted);

  return tile;
}

//
// TILES
//
Tile::Tile(TileType type, Vec2<float> position, int tileIndex, bool inserted)
    : type(type)
    , position(position)
    , texture(nullptr)
{
  switch (type) {
    case TILE_TYPE_TERRAIN:
      initializeFromTilemap(ResourceManager::loadTileMap("resources/terrain/terrain_tiles.png"), position, tileIndex, inserted);
      break;
    case TILE_TYPE_CRATE:
      initializeStaticTile(ResourceManager::loadTexture("resources/terrain/crate.png"), position, {0, 24}, inserted);
      break;
    case TILE_TYPE_GRASS:
      initializeFromTilemap(ResourceManager::loadTileMap("resources/decoration/grass/grass.png"), position, tileIndex, inserted);
      break;
    case TILE_TYPE_PLAYER_SETUP:
      initializeFromTilemap(ResourceManager::loadTexture("resources/character/setup_tiles.png"), position, tileIndex, inserted);
      break;
    case TILE_TYPE_ENEMY:
      initializeFromTilemap(ResourceManager::loadTexture("resources/enemy/setup_tile.png"), position, tileIndex, inserted);
      break;
    case TILE_TYPE_COIN:
      initializeFromTilemap(ResourceManager::loadTexture("resources/coins/coin_tiles.png"), position, tileIndex, inserted);
      break;
    case TILE_TYPE_FG_PALM:
      if(tileIndex == 1) { // 1 = small, 2 = large
        initializeStaticTile(ResourceManager::loadTexture("resources/terrain/palm_small/small_1.png"), position, {0, -38}, inserted);
      } else {
        initializeStaticTile(ResourceManager::loadTexture("resources/terrain/palm_large/large_1.png"), position, {0, -64}, inserted);
      }
      break;
    case TILE_TYPE_BG_PALM:
      initializeStaticTile(ResourceManager::loadTexture("resources/terrain/palm_bg/bg_palm_1.png"), position, {0, -64}, inserted);
      break;
    case TILE_TYPE_CONSTRAINT:
      initializeFromTilemap(ResourceManager::loadTexture("resources/enemy/setup_tile.png"), position, tileIndex, inserted);
      break;
  }
}

void Tile::update(Vec2<float> offset) {
    dstRect.x = position.x * TILE_SIZE - offset.x;
    dstRect.y = position.y * TILE_SIZE + this->offset.y;
}

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

void Tile::initializeStaticTile(SDL_Texture* texture, Vec2<float> position, Vec2<float> offset, bool inserted) {
  if(!texture) {
    Log::Error("Kunne ikke indlæse tile: passerede nullptr i tex");
    return;
  }

  float texW, texH;
  SDL_GetTextureSize(texture, &texW, &texH);

  if(inserted) {
    dstRect.x = position.x;
    dstRect.y = position.y;
  } else {
    dstRect.x = static_cast<float>(position.x * TILE_SIZE);
    dstRect.y = static_cast<float>(position.y * TILE_SIZE);
  }

  dstRect.w = texW;
  dstRect.h = texH;

  this->texture = texture;
  this->offset = offset;
  this->staticTile = true;
}

TileType Tile::getType() const {
  return type;
}


int Tile::getTileIndex() const {
  return currentTileIndex;
}

void Tile::initializeFromTilemap(SDL_Texture* tileMapTex, Vec2<float> position, int tileIndex, bool inserted) {
    if (!tileMapTex) {
        Log::Error("Kunne ikke indlæse tilemap: passerede nullptr i tileMapTex");
        return;
    }

    float tilesheetWidth = 0, tilesheetHeight = 0;
    SDL_GetTextureSize(tileMapTex, &tilesheetWidth, &tilesheetHeight);
    const int tilesPerRow = static_cast<int>(tilesheetWidth) / TILE_SIZE; // 256 / 64

    currentTileIndex = tileIndex;

    srcRect.x = static_cast<float>((tileIndex % tilesPerRow) * TILE_SIZE);
    srcRect.y = static_cast<float>((tileIndex / tilesPerRow) * TILE_SIZE);
    srcRect.w = TILE_SIZE;
    srcRect.h = TILE_SIZE;

    if(inserted) {
      dstRect.x = position.x;
      dstRect.y = position.y;
    } else {
      dstRect.x = static_cast<float>(position.x * TILE_SIZE);
      dstRect.y = static_cast<float>(position.y * TILE_SIZE);
    }

    dstRect.w = TILE_SIZE;
    dstRect.h = TILE_SIZE;

    this->texture = tileMapTex;
    this->staticTile = false;
}

void Tile::setTileIndex(int tileIndex) {
  if (!texture || staticTile) return;
  float tilesheetWidth = 0, tilesheetHeight = 0;
  SDL_GetTextureSize(texture, &tilesheetWidth, &tilesheetHeight);
  const int tilesPerRow = static_cast<int>(tilesheetWidth) / TILE_SIZE;

  currentTileIndex = tileIndex;

  srcRect.x = static_cast<float>((tileIndex % tilesPerRow) * TILE_SIZE);
  srcRect.y = static_cast<float>((tileIndex / tilesPerRow) * TILE_SIZE);
  srcRect.w = TILE_SIZE;
  srcRect.h = TILE_SIZE;
}
