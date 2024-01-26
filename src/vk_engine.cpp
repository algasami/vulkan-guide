
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include "vk_initializers.h"
#include "vk_types.h"

#include <chrono>
#include <thread>

constexpr bool bUseValidationLayers = true;

VulkanEngine *loadedEngine = nullptr;

void VulkanEngine::init() {

  assert(loadedEngine == nullptr);
  loadedEngine = this;
  // We initialize SDL and create a window with it.
  SDL_Init(SDL_INIT_VIDEO);

  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

  _window = SDL_CreateWindow("Vulkan Engine", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, _windowExtent.width,
                             _windowExtent.height, window_flags);

  // everything went fine
  _isInitialized = true;
}
void VulkanEngine::cleanup() {
  if (_isInitialized) {

    SDL_DestroyWindow(_window);
  }

  // clear pointer
  loadedEngine = nullptr;
}

void VulkanEngine::draw() {
  // nothing yet
}

void VulkanEngine::run() {
  SDL_Event e;
  bool bQuit = false;

  // main loop
  while (!bQuit) {
    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
      // close the window when user alt-f4s or clicks the X button
      if (e.type == SDL_QUIT)
        bQuit = true;
      if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
          stopRendering = true;
        } else if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
          stopRendering = false;
        }
      }
      // handles keyboard events
      else if (e.type == SDL_KEYDOWN) {
        // this adds the constant attribute to our keysym (locally)
        fmt::println("scan: {}", SDL_GetScancodeName(e.key.keysym.scancode));
        fmt::println("sym: {}", SDL_GetKeyName(e.key.keysym.sym));
      } else if (e.type == SDL_KEYUP) {
        SDL_Keysym const &keysym = e.key.keysym;
      }
    }
    if (stopRendering) {
      // halt and stand by
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }
    draw();
  }
}

VulkanEngine &VulkanEngine::Get() {
  assert(loadedEngine != nullptr);
  return *loadedEngine;
}