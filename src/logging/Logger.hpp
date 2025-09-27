#pragma once
#include <string>
#include <format>
#include "SDL3/SDL.h"

namespace Log {
  enum class Level {
    Debug,
    Info,
    Warn,
    Error,
    Critical
  };

  void Init();

  template<typename... Args>
  void Message(Level level, std::string_view fmt, Args&&... args) {
      std::string msg = std::vformat(fmt, std::make_format_args(args...));
      if (level == Level::Critical) {
          msg += " (";
          msg += SDL_GetError();
          msg += ")";
      }

      switch(level) {
          case Level::Debug:    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str()); break;
          case Level::Info:     SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str()); break;
          case Level::Warn:     SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str()); break;
          case Level::Error:    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str()); break;
          case Level::Critical: SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", msg.c_str()); break;
      }
  }

  template<typename... Args>
  void Error(std::string_view fmt, Args&&... args) {
    Message(Level::Error, fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void Debug(std::string_view fmt, Args&&... args) {
    Message(Level::Debug, fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void Info(std::string_view fmt, Args&&... args) {
    Message(Level::Info, fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void Warn(std::string_view fmt, Args&&... args) {
    Message(Level::Warn, fmt, std::forward<Args>(args)...);
  }

  template<typename... Args>
  void Critical(std::string_view fmt, Args&&... args) {
    Message(Level::Critical, fmt, std::forward<Args>(args)...);
  }
}