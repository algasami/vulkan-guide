// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>

class VulkanEngine {
public:
  bool _isInitialized{false};
  int _frameNumber{0};
  bool stopRendering{false};

  VkExtent2D _windowExtent{1700, 900};

  struct SDL_Window *_window{nullptr};

  // singleton fetcher
  static VulkanEngine &Get();

  // initializes everything in the engine
  void init();

  // shuts down the engine
  void cleanup();

  // draw loop
  void draw();

  // run main loop
  void run();

  VkInstance _instance;                      // library handle
  VkDebugUtilsMessengerEXT _debug_messenger; // output handle
  VkPhysicalDevice _chosenGPU;               // discrete metal
  VkDevice _device;                          // logical device
  VkSurfaceKHR _surface;                     // window surface

private:
  void init_vulkan();
  void init_swapchain();
  void init_commands();
  void init_sync_structures();
};
