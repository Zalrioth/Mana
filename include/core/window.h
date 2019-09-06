//https://www.codementor.io/a_hathon/building-and-using-dlls-in-c-d7rrd4caz
//http://www.syntaxbook.com/post/25995O2-how-to-measure-time-in-milliseconds-using-ansi-c

#pragma once
#ifndef WINDOW_H_
#define WINDOW_H_

#include "core/common.h"
#include "graphics/mesh.h"
#include "graphics/model.h"
#include "graphics/texture.h"

#ifdef NDEBUG
static const bool enable_validation_layers = false;
#else
static const bool enable_validation_layers = true;
#endif

static const char *const validation_layers[] = {"VK_LAYER_LUNARG_standard_validation"};
static const char *const device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#define MAX_FRAMES_IN_FLIGHT 2
#define MAX_SWAP_CHAIN_FRAMES MAX_FRAMES_IN_FLIGHT + 1

#define CREATE_WINDOW_ERROR 1
#define CREATE_INSTANCE_ERROR 2
#define SETUP_DEBUG_MESSENGER_ERROR 3
#define CREATE_SURFACE_ERROR 4
#define PICK_PHYSICAL_DEVICE_ERROR 5
#define CREATE_LOGICAL_DEVICE_ERROR 6
#define CREATE_SWAP_CHAIN_ERROR 7
#define CREATE_IMAGE_VIEWS_ERROR 8
#define CREATE_RENDER_PASS_ERROR 9
#define CREATE_GRAPHICS_PIPELINE_ERROR 10
#define CREATE_FRAME_BUFFER_ERROR 11
#define CREATE_COMMAND_POOL_ERROR 12
#define CREATE_COMMAND_BUFFER_ERROR 13
#define CREATE_SYNC_OBJECT_ERROR 14

struct QueueFamilyIndices
{
    uint32_t graphics_family;
    uint32_t present_family;
};

struct UniformBufferObject
{
    alignas(16) mat4 model;
    alignas(16) mat4 view;
    alignas(16) mat4 proj;
};

struct SwapChainSupportDetails
{
    struct VkSurfaceCapabilitiesKHR capabilities;
    struct VkSurfaceFormatKHR formats[SMALL_BUFFER];
    enum VkPresentModeKHR present_modes[SMALL_BUFFER];
};

struct Window
{
    GLFWwindow *glfw_window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSwapchainKHR swap_chain;
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    VkRenderPass render_pass;
    VkDescriptorSetLayout descriptor_set_layout;
    VkPipelineLayout pipeline_layout;
    VkPipeline graphics_pipeline;
    VkImage swap_chain_images[MAX_SWAP_CHAIN_FRAMES];
    VkImageView swap_chain_image_views[MAX_SWAP_CHAIN_FRAMES];
    VkFramebuffer swap_chain_framebuffers[MAX_SWAP_CHAIN_FRAMES];
    VkDebugUtilsMessengerEXT debug_messenger;
    VkCommandPool command_pool;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet descriptor_sets[MAX_SWAP_CHAIN_FRAMES];

    struct Mesh *image_mesh;
    struct Texture *image_texture;

    VkBuffer uniform_buffers[MAX_SWAP_CHAIN_FRAMES];
    VkDeviceMemory uniform_buffers_memory[MAX_SWAP_CHAIN_FRAMES];

    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    VkBuffer index_buffer;
    VkDeviceMemory index_buffer_memory;

    VkImage depth_image;
    VkDeviceMemory depth_image_memory;
    VkImageView depth_image_view;

    VkCommandBuffer command_buffers[MAX_SWAP_CHAIN_FRAMES];
    VkSemaphore image_available_semaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore render_finished_semaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence in_flight_fences[MAX_FRAMES_IN_FLIGHT];
    size_t current_frame;
    struct QueueFamilyIndices indices;
    int width;
    int height;
    bool framebuffer_resized;
};

int window_init(struct Window *window, int width, int height);
void window_delete(struct Window *game_window);
int create_glfw_window(struct Window *game_window, int width, int height);
bool check_validation_layer_support();
void recreate_swap_chain(struct Window *window);

int create_window(struct Window *window, int width, int height);
int create_instance(struct Window *window);
int setup_debug_messenger(struct Window *window);
int create_surface(struct Window *window);
int pick_physical_device(struct Window *window);
int create_logical_device(struct Window *window);
int create_swap_chain(struct Window *window);
int create_image_views(struct Window *window);
int create_render_pass(struct Window *window);
int create_descriptor_set_layout(struct Window *window);
int create_graphics_pipeline(struct Window *window);
int create_framebuffers(struct Window *window);
int create_command_pool(struct Window *window);
int create_depth_resources(struct Window *window);
int create_vertex_buffer(struct Window *window);
int create_index_buffer(struct Window *window);
int create_uniform_buffers(struct Window *window);
int create_descriptor_pool(struct Window *window);
int create_descriptor_sets(struct Window *window);

int create_command_buffers(struct Window *window);
int create_sync_objects(struct Window *window);

bool is_device_suitable(struct Window *window, VkPhysicalDevice device);
void copy_buffer(struct Window *window, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
void cleanup_swap_chain(struct Window *window);
VkVertexInputBindingDescription get_binding_description();
void get_attribute_descriptions(VkVertexInputAttributeDescription *attribute_descriptions);

#endif // WINDOW_H_