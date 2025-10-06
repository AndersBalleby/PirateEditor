#include "Editor.hpp"

Editor::Editor()
  : scene_manager(0, "TestScene")
{
  Log::Info("Initialiserer editor");
}

void Editor::run(SDL_State& state) {
  scene_manager.update(state);
  scene_manager.draw(state.renderer);
}
