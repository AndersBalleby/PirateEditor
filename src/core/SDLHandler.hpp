#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include "../core/TextRenderer.hpp"

struct SDLState {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::unique_ptr<TextRenderer> tRenderer;
  bool running;
  int winWidth;
  int winHeight;
  double deltaTime;
public:
  SDLState();
  ~SDLState();

  // Slet både copy og move
  SDLState(const SDLState&) = delete;
  SDLState &operator=(const SDLState&) = delete;
  
  SDLState(SDLState &&other) = delete;
  SDLState &operator=(SDLState &&other) = delete;

  // Getters
  SDL_Window *getWindow() const { return this->window; }
  SDL_Renderer *getRenderer() const { return this->renderer; }
  TextRenderer *getTextRenderer() const { return this->tRenderer.get(); }
  bool isRunning() const { return this->running; }
  int getWindowWidth() const { return this->winWidth; }
  int getWindowHeight() const { return this->winHeight; }
  double getDeltaTime() const { return this->deltaTime; }

  // Setters
  void setRunning(bool running) { this->running = running; }
  void setWindowWidth(int winWidth) { this->winWidth = winWidth; }
  void setWindowHeight(int winHeight) { this->winHeight = winHeight; }
  void setDeltaTime(double deltaTime) { this->deltaTime = deltaTime; }
};

class SDLHandler {
private:
  SDLState state;
  uint64_t prevCounter;
  uint64_t freq;
public:
  SDLHandler();
  ~SDLHandler() = default;
  
  // Også slet copy og move her - vi skal kun have 1 instance
  SDLHandler(const SDLHandler&) = delete;
  SDLHandler &operator=(const SDLHandler&) = delete;
  
  SDLHandler(SDLHandler &&other) = delete;
  SDLHandler &operator=(SDLHandler &&other) = delete;

  // Wrapper funktioner for SDLState
  SDL_Window *getWindow() const { return state.getWindow(); }
  SDL_Renderer *getRenderer() const { return state.getRenderer(); }
  TextRenderer *getTextRenderer() const { return state.getTextRenderer(); }
  bool isRunning() const { return state.isRunning(); }
  int getWindowWidth() const { return state.getWindowWidth(); }
  int getWindowHeight() const { return state.getWindowHeight(); }
  double getDeltaTime() const { return state.getDeltaTime(); }
   
  void handleEvents();
  void tickDeltaTime();
};

