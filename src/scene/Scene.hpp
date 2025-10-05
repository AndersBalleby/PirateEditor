#pragma once

#include <string>
#include <format>
#include <filesystem>
#include <vector>

#include "logging/Logger.hpp"
#include "core/TileManager.hpp"
#include "io/utils.hpp"
#include "sdl/SDL_Handler.hpp"
#include "Background.hpp"

struct SceneLayout {
  Utils::Layout bgPalmsLayout;
  Utils::Layout coinsLayout;
  Utils::Layout constraintsLayout;
  Utils::Layout cratesLayout;
  Utils::Layout enemiesLayout;
  Utils::Layout fgPalmsLayout;
  Utils::Layout grassLayout;
  Utils::Layout playerLayout;
  Utils::Layout terrainLayout;

  [[nodiscard]] static Utils::Layout LoadLevelLayout(unsigned int level, const std::string& name);

  explicit SceneLayout(unsigned int level);
};

struct SceneTiles {
  std::vector<Tile*> terrainTiles;
  std::vector<Tile*> crateTiles;
  std::vector<Tile*> grassTiles;

  static std::vector<Tile*> LoadTiles(TileType type, const Utils::Layout& layout);
  void DrawTiles(SDL_Renderer* renderer) const;
  void UpdateTiles(SDL_State& state, float mapHeight, float cameraX);

  explicit SceneTiles(const SceneLayout& layout);
};

class Scene {
  public:
    Scene(unsigned int level, const std::string& name);
    ~Scene() = default;

    Scene(const Scene&) = default;
    Scene(Scene&&) noexcept = default;

    void update(SDL_State& state) noexcept;
    void draw(SDL_Renderer* renderer) const noexcept;
    void handleInput(const SDL_Event& event, float deltaTime) noexcept;

    void saveScene(const std::filesystem::path& path);

  private:
    // Camera - måske lave dette som en class på et tidspunkt
    float cameraX = 0.0f;
    const float scrollSpeed = 750.0f;

    std::string name;
    unsigned int level;
    SceneLayout layout;
    SceneTiles tiles;
    Background bg;
};
