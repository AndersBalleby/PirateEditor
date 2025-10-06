#include "Editor.hpp"

Editor::Editor()
  : scene_manager(0, "TestScene")
{
  Log::Info("Initialiserer editor");
}

void Editor::update(SDL_State& state) {
  scene_manager.update(state);
}

void Editor::draw(SDL_Renderer* renderer) const {
  scene_manager.draw(renderer);
}

void Editor::run(SDL_State& state) {
  this->update(state);
  this->draw(state.renderer);
}
