//https://www.codementor.io/a_hathon/building-and-using-dlls-in-c-d7rrd4caz
//http://www.syntaxbook.com/post/25995O2-how-to-measure-time-in-milliseconds-using-ansi-c

#pragma once
#ifndef WINDOW_H_
#define WINDOW_H_

#include "datastructures/vector.h"

#include "core/common.h"

#define FPS_COUNT 10

#ifdef NDEBUG
static const bool enableValidationLayers = false;
#else
static const bool enableValidationLayers = true;
#endif

static const char* const validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
static const char* const deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#define MAX_FRAMES_IN_FLIGHT 2

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{

    //std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    //VkResult (*func)(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*) = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    //void* func = vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

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
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkImage swapChainImages[3];
    VkImageView swapChainImageViews[3];
    VkFramebuffer swapChainFramebuffers[3];
    VkDebugUtilsMessengerEXT debugMessenger;
    VkCommandPool commandPool;
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