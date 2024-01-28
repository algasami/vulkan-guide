//> includes
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_initializers.h>
#include <vk_types.h>

#include "VkBootstrap.h"

#include <chrono>
#include <thread>

constexpr bool bUseValidationLayers = true;
VulkanEngine *loadedEngine = nullptr;

VulkanEngine &VulkanEngine::Get() { return *loadedEngine; }

void VulkanEngine::init() {
  // only one engine initialization is allowed with the application.
  assert(loadedEngine == nullptr);
  loadedEngine = this;

  // We initialize SDL and create a window with it.
  SDL_Init(SDL_INIT_VIDEO);

  SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

  _window = SDL_CreateWindow("Vulkan Engine", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, _windowExtent.width,
                             _windowExtent.height, windowFlags);

  init_vulkan();

  init_swapchain();

  init_commands();

  init_sync_structures();

  // everything went fine
  _isInitialized = true;
}

void VulkanEngine::cleanup() {
  if (_isInitialized) {
    destroy_swapchain();
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyDevice(_device, nullptr);

    vkb::destroy_debug_utils_messenger(_instance, _debugMessenger);
    vkDestroyInstance(_instance, nullptr);
    // we don't need to destroy physical devices due to their descriptive nature
    // (descriptor objects)
    SDL_DestroyWindow(_window);
  }

  // clear engine pointer
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
        }
        if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
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

    // do not draw if we are minimized
    if (stopRendering) {
      // throttle the speed to avoid the endless spinning
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    draw();
  }
}

void VulkanEngine::init_vulkan() {
  vkb::InstanceBuilder builder;
  auto instanceRet = builder.set_app_name("Vk App")
                         .request_validation_layers(bUseValidationLayers)
                         .use_default_debug_messenger()
                         .require_api_version(1, 3, 0)
                         .build();
  vkb::Instance vkbInstance = instanceRet.value();

  _instance = vkbInstance.instance;
  _debugMessenger = vkbInstance.debug_messenger;

  SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

  // vulkan 1.3 features
  VkPhysicalDeviceVulkan13Features features{};
  features.dynamicRendering = true;
  features.synchronization2 = true;

  // vulkan 1.2 features
  VkPhysicalDeviceVulkan12Features features12{};
  features12.bufferDeviceAddress = true;
  features12.descriptorIndexing = true;

  // Physical Device = GPU's properties
  vkb::PhysicalDeviceSelector selector{vkbInstance};
  vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1, 3)
                                           .set_required_features_13(features)
                                           .set_required_features_12(features12)
                                           .set_surface(_surface)
                                           .select()
                                           .value();

  vkb::DeviceBuilder deviceBuilder{physicalDevice};

  vkb::Device vkbDevice = deviceBuilder.build().value();

  _device = vkbDevice.device;
  _chosenGPU = physicalDevice.physical_device;

  // vkbootstrap for queue fetching
  _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
  _graphicsQueueFamily =
      vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}
void VulkanEngine::init_commands() {}
void VulkanEngine::init_sync_structures() {}

void VulkanEngine::create_swapchain(uint32_t width, uint32_t height) {
  vkb::SwapchainBuilder swapchainBuilder{_chosenGPU, _device, _surface};

  _swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

  vkb::Swapchain vkbSwapchain =
      swapchainBuilder
          .set_desired_format(VkSurfaceFormatKHR{
              .format = _swapchainImageFormat,
              .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
          // FIFO_RELAXED
          // This means that our engine allows tearing when our current frame
          // rate is less than that of the monitor's, while enabling VSync when
          // it's no less than the monitor's.
          .set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR)
          .set_desired_extent(width, height)
          .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
          .build()
          .value();

  _swapchainExtent = vkbSwapchain.extent;

  // since VkSwapchain, VkImage, VkImageView are all handles(pointers) to hidden
  // structs, we can directly copy vectors of them without worrying about mem
  // leaks (as they are pre-allocated by the vulkan instance)
  _swapchain = vkbSwapchain.swapchain;
  _swapchainImages = vkbSwapchain.get_images().value();
  _swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanEngine::init_swapchain() {
  create_swapchain(_windowExtent.width, _windowExtent.height);
}
void VulkanEngine::destroy_swapchain() {
  vkDestroySwapchainKHR(_device, _swapchain, nullptr);

  // swapchain resources (image, image view) should also be destroyed
  // note image views are handlers of images, so we can just destroy image
  // views, and images would be destroyed simultaneously.
  for (VkImageView &const imageView : _swapchainImageViews) {
    vkDestroyImageView(_device, imageView, nullptr);
  }
}