//https://www.codementor.io/a_hathon/building-and-using-dlls-in-c-d7rrd4caz
//http://www.syntaxbook.com/post/25995O2-how-to-measure-time-in-milliseconds-using-ansi-c

#pragma once
#ifndef WINDOW_H_
#define WINDOW_H_

#include "core/common.h"
#include <cglm/cglm.h>

#ifdef NDEBUG
static const bool enableValidationLayers = false;
#else
static const bool enableValidationLayers = true;
#endif

static const char* const validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
static const char* const deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
};

//presentModes

//vector myVector
//vector_init(&myVector);
//void vector_init(vector*);

struct SwapChainSupportDetails {
    struct VkSurfaceCapabilitiesKHR capabilities;
    struct VkSurfaceFormatKHR formats[SMALL_BUFFER];
    enum VkPresentModeKHR presentModes[SMALL_BUFFER];
};

struct Window {
    GLFWwindow* glfwWindow;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkImage swapChainImages[MAX_SWAP_CHAIN_FRAMES];
    VkImageView swapChainImageViews[MAX_SWAP_CHAIN_FRAMES];
    VkFramebuffer swapChainFramebuffers[MAX_SWAP_CHAIN_FRAMES];
    VkDebugUtilsMessengerEXT debugMessenger;
    VkCommandPool commandPool;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    struct Vector imageVertices;
    struct Vector imageIndices;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkCommandBuffer commandBuffers[SMALL_BUFFER];
    VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
    size_t currentFrame;
    struct QueueFamilyIndices indices;
    int width;
    int height;
};

int init_window(struct Window* window, int width, int height);
void delete_window(struct Window* gameWindow);
int create_glfw_window(struct Window* gameWindow, int width, int height);
bool checkValidationLayerSupport();

#endif // WINDOW_H_