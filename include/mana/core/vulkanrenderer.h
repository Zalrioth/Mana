#pragma once
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include "mana/core/common.h"
#include "mana/graphics/graphicsutils.h"
#include "mana/graphics/mesh.h"
#include "mana/graphics/model.h"
#include "mana/graphics/swapchain.h"
#include "mana/graphics/texture.h"

#ifdef NDEBUG
static const bool enable_validation_layers = false;
#else
static const bool enable_validation_layers = true;
#endif

static const char* const validation_layers[] = {"VK_LAYER_LUNARG_standard_validation"};
static const char* const device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices {
  uint32_t graphics_family;
  uint32_t present_family;
};

struct UniformBufferObject {
  mat4 model;
  mat4 view;
  mat4 proj;
  //alignas(16) mat4 model;
  //alignas(16) mat4 view;
  //alignas(16) mat4 proj;
};

struct SwapChainSupportDetails {
  struct VkSurfaceCapabilitiesKHR capabilities;
  struct Vector formats;
  struct Vector present_modes;
  //struct VkSurfaceFormatKHR formats[SMALL_BUFFER];
  //enum VkPresentModeKHR present_modes[SMALL_BUFFER];
};

struct VulkanRenderer {
  struct GLFWwindow* glfw_window;
  struct VkInstance_T* instance;
  struct VkSurfaceKHR_T* surface;
  struct VkPhysicalDevice_T* physical_device;
  struct VkDevice_T* device;
  struct VkQueue_T* graphics_queue;
  struct VkQueue_T* present_queue;
  struct VkSwapchainKHR_T* swap_chain;
  enum VkFormat swap_chain_image_format;
  struct VkExtent2D swap_chain_extent;
  struct VkDebugUtilsMessengerEXT_T* debug_messenger;
  struct VkCommandPool_T* command_pool;
  struct VkCommandBuffer_T* command_buffers[MAX_SWAP_CHAIN_FRAMES];
  struct VkSemaphore_T* image_available_semaphores[MAX_FRAMES_IN_FLIGHT];
  struct VkSemaphore_T* render_finished_semaphores[MAX_FRAMES_IN_FLIGHT];
  struct VkFence_T* in_flight_fences[MAX_FRAMES_IN_FLIGHT];
  size_t current_frame;
  struct QueueFamilyIndices indices;
  bool framebuffer_resized;
  struct SwapChain* swapchain;
};

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

int vulkan_renderer_init(struct VulkanRenderer* vulkan_renderer, int width, int height);
void vulkan_renderer_delete(struct VulkanRenderer* vulkan_renderer);
bool check_validation_layer_support();
void recreate_swap_chain(struct VulkanRenderer* vulkan_renderer);
int create_instance(struct VulkanRenderer* vulkan_renderer);
int setup_debug_messenger(struct VulkanRenderer* vulkan_renderer);
int create_surface(struct VulkanRenderer* vulkan_renderer);
int pick_physical_device(struct VulkanRenderer* vulkan_renderer);
int create_logical_device(struct VulkanRenderer* vulkan_renderer);
int create_swap_chain(struct VulkanRenderer* vulkan_renderer, int width, int height);
//int create_image_views(struct VulkanRenderer* vulkan_renderer);
//int create_render_pass(struct VulkanRenderer* vulkan_renderer);
int create_descriptor_set_layout(struct VulkanRenderer* vulkan_renderer);

void create_color_attachment(struct VulkanRenderer* vulkan_renderer, struct VkAttachmentDescription* color_attachment);
void create_depth_attachment(struct VulkanRenderer* vulkan_renderer, struct VkAttachmentDescription* depth_attachment);
int create_depth_resources(struct VulkanRenderer* vulkan_renderer);

int create_framebuffers(struct VulkanRenderer* vulkan_renderer);
int create_command_pool(struct VulkanRenderer* vulkan_renderer);
//int create_vertex_buffer(struct VulkanRenderer* vulkan_renderer);
//int create_index_buffer(struct VulkanRenderer* vulkan_renderer);
//int create_uniform_buffers(struct VulkanRenderer* vulkan_renderer);
//int create_sprite_descriptor_pool(struct VulkanRenderer* vulkan_renderer);
int create_command_buffers(struct VulkanRenderer* vulkan_renderer);
int command_buffer_start(struct VulkanRenderer* vulkan_renderer, size_t i);
int command_buffer_reset(struct VulkanRenderer* vulkan_renderer, size_t i);
int command_buffer_end(struct VulkanRenderer* vulkan_renderer, size_t i);
int create_sync_objects(struct VulkanRenderer* vulkan_renderer);

void vulkan_sync_objects_cleanup(struct VulkanRenderer* vulkan_renderer);
void vulkan_command_pool_cleanup(struct VulkanRenderer* vulkan_renderer);
//void vulkan_descriptor_set_layout_cleanup(struct VulkanRenderer* vulkan_renderer);
void vulkan_swap_chain_cleanup(struct VulkanRenderer* vulkan_renderer);
void vulkan_device_cleanup(struct VulkanRenderer* vulkan_renderer);
void vulkan_surface_cleanup(struct VulkanRenderer* vulkan_renderer);
void vulkan_debug_cleanup(struct VulkanRenderer* vulkan_renderer);
void window_cleanup(struct VulkanRenderer* vulkan_renderer);

bool is_device_suitable(struct VulkanRenderer* vulkan_renderer, VkPhysicalDevice device);
void copy_buffer(struct VulkanRenderer* vulkan_renderer, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

void cleanup_swap_chain(struct VulkanRenderer* vulkan_renderer);
VkVertexInputBindingDescription get_binding_description();
void get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions);
VkFormat find_depth_format(struct VulkanRenderer* vulkan_renderer);

#endif  // VULKAN_RENDERER_H
