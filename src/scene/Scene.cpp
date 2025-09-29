#include "Scene.hpp"

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
