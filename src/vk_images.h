
#pragma once

#include "vk_initializers.h"
#include "vk_types.h"


namespace vkutil {

void transition_image(VkCommandBuffer cmd, VkImage image,
                      VkImageLayout currentLayout, VkImageLayout newLayout);
};