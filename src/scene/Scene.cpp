#include "Scene.hpp"

Scene::Scene(const std::string& name) {
  this->name = name;

  Log::Info("Indlæste scene \"{}\" successfuldt", name);
}

Scene::~Scene() {}

void Scene::update() {};
void Scene::draw() {};
void Scene::saveScene(const std::string& path) {};
