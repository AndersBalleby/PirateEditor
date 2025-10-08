#pragma once

#include <array>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <vector>

#include "logging/Logger.hpp"
#include "core/TileManager.hpp"
#include "io/utils.hpp"
#include "sdl/SDL_Handler.hpp"
#include "Background.hpp"

namespace Scene {
  using TileGroup = std::vector<Tile*>;

  void UpdateTileGroup(TileGroup& group, float mapOffsetY, float cameraX);
  void DrawTileGroup(TileGroup& group, SDL_Renderer* renderer);

struct Layout {
  Utils::TileLayer bgPalmsLayout;
  Utils::TileLayer coinsLayout;
  Utils::TileLayer constraintsLayout;
  Utils::TileLayer cratesLayout;
  Utils::TileLayer enemiesLayout;
  Utils::TileLayer fgPalmsLayout;
  Utils::TileLayer grassLayout;
  Utils::TileLayer playerSetupLayout;
  Utils::TileLayer terrainLayout;
  Utils::TileLayer constraintLayout;

  [[nodiscard]] static Utils::TileLayer LoadLevelLayout(unsigned int level, const std::string& name);

  explicit Layout(unsigned int level);
};

struct Tiles {
  TileGroup terrainTiles;
  TileGroup crateTiles;
  TileGroup grassTiles;
  TileGroup playerSetupTiles;
  TileGroup enemyTiles;
  TileGroup coinsTiles;
  TileGroup fgPalmsTiles;
  TileGroup bgPalmsTiles;
  TileGroup constraintTiles;

  // Vigtigt: Dette er også rækkefølgen de opdateres og tegnes i
  std::array<TileGroup*, 9> allGroups {
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

  std::unordered_map<long long, std::vector<Tile*>> tileLookup;
  static inline long long makeTileKey(int x, int y);

  static TileGroup LoadTiles(TileType type, const Utils::TileLayer& layout, std::unordered_map<long long, std::vector<Tile*>>& lookup);
  void DrawTiles(SDL_Renderer* renderer) const;
  void UpdateTiles(SDL_State& state, float mapHeight, float cameraX);
  void RemoveTile(int gridX, int gridY, bool allTiles = false);

  explicit Tiles(const Layout& layout);
};

class Manager {
  public:
    Manager(unsigned int level, const std::string& name);
    ~Manager() = default;

    Manager(const Manager&) = default;
    Manager(Manager&&) noexcept = default;

    void update(SDL_State& state) noexcept;
    void draw(SDL_Renderer* renderer) const noexcept;
    void saveScene(const std::filesystem::path& path);

    void removeTileAt(int gridX, int gridY);
    void removeLayerTiles(int gridX, int gridY);

  private:
    // Camera - måske lave dette som en class på et tidspunkt
    const float scrollSpeed = 750.0f;

    std::string name;
    unsigned int level;
    Layout layout;
    Tiles tiles;
    Background bg;
};

};
