// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "vk_descriptors.h"
#include "vk_types.h"

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
  void draw_background(VkCommandBuffer);

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
  VmaAllocator _allocator;

  AllocatedImage _drawImage;
  VkExtent2D _drawExtent;
  // _drawExtent is different from _swapchainExtent
  // we never draw directly to swapchain so we use another image as a proxy

  DescriptorAllocator globalDescriptorAllocator;

  VkDescriptorSet _drawImageDescriptors;
  VkDescriptorSetLayout _drawImageDescriptorLayout;
  // these are for shader bindings and sending data to GPUs

  VkPipeline _gradientPipeline;
  VkPipelineLayout _gradientPipelineLayout;
  // for loading shader

  VkFence _immFence;
  VkCommandBuffer _immCommandBuffer;
  VkCommandPool _immCommandPool;

  void immediate_submit(std::function<void(VkCommandBuffer cmd)> &&function);
  // right value function (avoid wasting memory)
  // for immediate submit imgui

private:
  void init_vulkan();
  void init_swapchain();
  void init_commands();
  void init_sync_structures();
  void init_descriptors();

  void init_pipelines();
  void init_background_pipelines();

  void init_imgui();
  void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

  void create_swapchain(uint32_t width, uint32_t height);
  void destroy_swapchain();
};
