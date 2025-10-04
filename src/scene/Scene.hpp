#pragma once

#include <string>
#include <format>
#include <filesystem>

#include "logging/Logger.hpp"
#include "io/utils.hpp"
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

class Scene {
  public:
    Scene(unsigned int level, const std::string& name);
    ~Scene() = default;

    Scene(const Scene&) = default;
    Scene(Scene&&) noexcept = default;

    void update(float deltaTime) noexcept;
    void draw(SDL_Renderer* renderer) const noexcept;

    void saveScene(const std::filesystem::path& path);

  private:
    unsigned int level;
    std::string name;
    SceneLayout layout;
    Background bg;
};
