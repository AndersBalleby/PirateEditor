#include "Logger.hpp"

namespace Log {
  void Init() {
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
  }
}
