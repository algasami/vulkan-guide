// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>

struct DeletionStack {

  std::stack<std::function<void()>> deletors;

  void push_function(std::function<void()> &&function) {
    deletors.push(function);
  }

  void flush() {
    while (!deletors.empty()) {
      deletors.top()();
      deletors.pop();
    }
  }
};

struct FrameData {
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;
  VkSemaphore _swapchainSemaphore, _renderSemaphore; // GPU <-> GPU fence
  // _swapchainSemaphore: render command wait on swapchain
  // _renderSemaphore: control presenting image
  VkFence _renderFence; // GPU -> CPU fence
  DeletionStack _deletionStack;
};

// this means that we render one frame and prepare another one simultaneously
constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
public:
  bool _isInitialized{false};
  int _frameNumber{0};
  bool stopRendering{false};
  VkExtent2D _windowExtent{1700, 900};

  struct SDL_Window *_window{nullptr};

  static VulkanEngine &Get();

  // initializes everything in the engine
  void init();

  // shuts down the engine
  void cleanup();

  // draw loop
  void draw();

  // run main loop
  void run();
  VkInstance _instance;
  VkDebugUtilsMessengerEXT _debugMessenger; // debug output
  VkPhysicalDevice _chosenGPU;              // discrete GPU
  VkDevice _device;                         // logical device
  VkSurfaceKHR _surface;                    // window surface
  VkSwapchainKHR _swapchain;
  VkFormat _swapchainImageFormat;

  std::vector<VkImage> _swapchainImages;
  std::vector<VkImageView> _swapchainImageViews;
  VkExtent2D _swapchainExtent;

  FrameData _frames[FRAME_OVERLAP]; // no direct access
  FrameData &get_current_frame() {  // getter
    return _frames[_frameNumber % FRAME_OVERLAP];
  }

  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;
  DeletionStack _deletionStack;

private:
  void init_vulkan();
  void init_swapchain();
  void init_commands();
  void init_sync_structures();

  void create_swapchain(uint32_t width, uint32_t height);
  void destroy_swapchain();
};
