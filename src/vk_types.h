// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.
#pragma once

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <stack>
#include <string>
#include <vector>


#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <fmt/core.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define VK_CHECK(x)                                                            \
  do {                                                                         \
    VkResult err = x;                                                          \
    if (err) {                                                                 \
      fmt::println("Detected Vulkan error: {}", string_VkResult(err));         \
      abort();                                                                 \
    }                                                                          \
  } while (0)