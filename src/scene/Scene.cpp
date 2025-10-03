#include "Scene.hpp"

SceneLayout::SceneLayout(unsigned int level) {
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

Utils::Layout SceneLayout::LoadLevelLayout(unsigned int level, const std::string& name) {
  Utils::Layout result = Utils::LoadCSVFile(std::format("levels/{0}/level_{0}_{1}.csv", level, name));
  if(result.empty()) {
    Log::Error("Kunne ikke validere resultat fra Utils::LoadCSVFile ({})", name);
    return {};
  }  

  return result;
}

Scene::Scene(unsigned int level, const std::string& name) 
  : level(level)
  , name(name)
  , layout(SceneLayout(level))  
{
  Log::Info("Indlæste scene \"{}\" successfuldt", name);
}

void Scene::update() noexcept {
  
};

void Scene::draw(SDL_Renderer* renderer) const noexcept {

};

void Scene::saveScene(const std::filesystem::path& path) {};
