#include "TileManager.hpp"

//
// TILES
//
Tile::Tile(TileType type) {
    this->type = type;
}

//
// TILEGROUP
//
TileGroup::TileGroup(const std::string& id) : id(id) {}

//
// TILEMANAGER
//
TileManager::TileManager() {

}

bool TileManager::init() {
  return true;
}
