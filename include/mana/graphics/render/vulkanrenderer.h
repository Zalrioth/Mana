#pragma once
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "mana/core/memoryallocator.h"
//
#define _CRT_SECURE_NO_DEPRECATE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cstorage/cstorage.h>
#include <stdalign.h>
#include <vulkan/vulkan.h>

#include "mana/core/corecommon.h"
#include "mana/core/vulkancore.h"
#include "mana/graphics/entities/model.h"
#include "mana/graphics/graphicscommon.h"
#include "mana/graphics/render/gbuffer.h"
#include "mana/graphics/render/postprocess.h"
#include "mana/graphics/render/swapchain.h"
#include "mana/graphics/utilities/graphicsutils.h"
#include "mana/graphics/utilities/mesh.h"
#include "mana/graphics/utilities/texture.h"

enum VULKAN_RENDERER_STATUS { VULKAN_RENDERER_CREATE_WINDOW_ERROR = 0,
                              VULKAN_RENDERER_SUCCESS = 1,
                              VULKAN_RENDERER_CREATE_INSTANCE_ERROR = 2,
                              VULKAN_RENDERER_SETUP_DEBUG_MESSENGER_ERROR = 3,
                              VULKAN_RENDERER_CREATE_SURFACE_ERROR = 4,
                              VULKAN_RENDERER_PICK_PHYSICAL_DEVICE_ERROR = 5,
                              VULKAN_RENDERER_CREATE_LOGICAL_DEVICE_ERROR = 6,
                              VULKAN_RENDERER_CREATE_SWAP_CHAIN_ERROR = 7,
                              VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR = 8,
                              VULKAN_RENDERER_CREATE_RENDER_PASS_ERROR = 9,
                              VULKAN_RENDERER_CREATE_GRAPHICS_PIPELINE_ERROR = 10,
                              VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR = 11,
                              VULKAN_RENDERER_CREATE_COMMAND_POOL_ERROR = 12,
                              VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR = 13,
                              VULKAN_RENDERER_CREATE_SYNC_OBJECT_ERROR = 14
};

int vulkan_renderer_init(struct VulkanState* vulkan_state, int width, int height);
int vulken_renderer_glfw_init(struct VulkanState* vulkan_state);
bool vulken_renderer_check_validation_layer_support();
void vulken_renderer_recreate_swap_chain(struct VulkanState* vulkan_state);
int vulken_renderer_create_surface(struct VulkanState* vulkan_state);
static bool vulkan_renderer_can_device_present(struct VulkanState* vulkan_state, VkPhysicalDevice device);

void vulken_renderer_create_color_attachment(struct VulkanState* vulkan_state, struct VkAttachmentDescription* color_attachment);
void vulken_renderer_create_depth_attachment(struct VulkanState* vulkan_state, struct VkAttachmentDescription* depth_attachment);

void copy_buffer(struct VulkanState* vulkan_state, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
static VkSampleCountFlagBits vulkan_renderer_get_max_usable_sample_count(struct VulkanState* vulkan_state);

#endif  // VULKAN_RENDERER_H
