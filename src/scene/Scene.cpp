#include "Scene.hpp"

Scene::Scene(unsigned int level, const std::string& name) 
  : level(level)
  , name(name)
  , layout(SceneLayout(level))  
{
  Log::Info("Indlæste scene \"{}\" successfuldt", name);
}

void Scene::update() noexcept {};
void Scene::draw() const noexcept {};

void Scene::saveScene(const std::filesystem::path& path) {};
