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
            // Initialize terrain tile properties
            initializeFromTilemap(ResourceManager::loadTileMap("resources/terrain/terrain_tiles.png"), position, tileIndex);
            break;
    }
}

void Tile::update(float offsetX) {
  dstRect.x -= offsetX;
}

void Tile::draw(SDL_Renderer *renderer) const {
  if (!texture) {
      Log::Error("Kunne ikke tegne tile: passerede nullptr i texture");
      return;
    }

  SDL_RenderTexture(renderer, texture, &srcRect, &dstRect);

}

const int TILE_SIZE = 64;
const float OFFSET_Y = 100.0f;
void Tile::initializeFromTilemap(SDL_Texture* tileMapTex, Vec2<float> position, int tileIndex) {
    if (!tileMapTex) {
        Log::Error("Kunne ikke indlæse tilemap: passerede nullptr i tileMapTex");
        return;
    }

    float tilesheetWidth = 0, tilesheetHeight = 0;
    SDL_GetTextureSize(tileMapTex, &tilesheetWidth, &tilesheetHeight);

    const float EPS = 0.5f; // lille padding til at forhindre bleeding
    const int tilesPerRow = 4; // 256 / 64
    SDL_FRect srcRect;
    srcRect.x = static_cast<float>((tileIndex % tilesPerRow) * TILE_SIZE) + EPS;
    srcRect.y = static_cast<float>((tileIndex / tilesPerRow) * TILE_SIZE) + EPS;
    srcRect.w = TILE_SIZE - 2*EPS;
    srcRect.h = TILE_SIZE - 2*EPS;

    SDL_FRect dstRect;
    dstRect.x = static_cast<float>(position.x * TILE_SIZE);
    dstRect.y = static_cast<float>(position.y * TILE_SIZE) + OFFSET_Y;
    dstRect.w = TILE_SIZE;
    dstRect.h = TILE_SIZE;

    this->texture = tileMapTex;
    this->srcRect = srcRect;
    this->dstRect = dstRect;
}
