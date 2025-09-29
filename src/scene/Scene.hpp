#pragma once

#include <string>
#include <format>
#include <filesystem>

#include "logging/Logger.hpp"
#include "io/utils.hpp"

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

  [[nodiscard]] static Utils::Layout LoadLevelLayout(unsigned int level, const std::string& name) {
    Utils::Layout result = Utils::LoadCSVFile(std::format("levels/{0}/level_{0}_{1}.csv", level, name));
    if(result.size() == 0 || result.empty()) {
      Log::Error("Kunne ikke validere resultat fra Utils::LoadCSVFile ({})", name);
      return {};
    }  

    return Utils::LoadCSVFile(std::format("levels/{0}/level_{0}_{1}.csv", level, name));
  }

  SceneLayout(unsigned int level) {
    bgPalmsLayout     = LoadLevelLayout(level, "bg_palms");
    coinsLayout       = LoadLevelLayout(level, "coins");
    constraintsLayout = LoadLevelLayout(level, "constraints");
    cratesLayout      = LoadLevelLayout(level, "crates");
    enemiesLayout     = LoadLevelLayout(level, "enemies");
    fgPalmsLayout     = LoadLevelLayout(level, "fg_palms");
    grassLayout       = LoadLevelLayout(level, "grass");
    playerLayout      = LoadLevelLayout(level, "player");
    terrainLayout     = LoadLevelLayout(level, "terrain");
  }
};

class Scene {
  public:
    Scene(unsigned int level, const std::string& name);
    ~Scene() = default;

    Scene(const Scene&) = default;
    Scene(Scene&&) noexcept = default;
    Scene& operator=(const Scene&) = default;
    Scene& operator=(Scene&&) noexcept = default;

    void update() noexcept;
    void draw(SDL_Renderer* renderer) const noexcept;
    
    void saveScene(const std::filesystem::path& path);

  private:
    unsigned int level;
    std::string name;
    SceneLayout layout;
};