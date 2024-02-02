
#pragma once

#include "vk_initializers.h"
#include "vk_types.h"

namespace vkutil {

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout,
                      VkImageLayout newLayout);
void copy_image_to_image(VkCommandBuffer cmd, VkImage image, VkImage destination,
                         VkExtent2D srcSize, VkExtent2D dstSize);
}; // namespace vkutil
