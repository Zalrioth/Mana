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

static const char* const validationLayers[] = { "VK_LAYER_LUNARG_standard_validation\0" };

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
    VkResult (*func)(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*) = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
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
    VkImage swapChainImages[SMALL_BUFFER];
    VkDebugUtilsMessengerEXT debugMessenger;
    struct QueueFamilyIndices indices;
    int width;
    int height;
};

int init_window(struct Window* gameWindow);
void delete_window(struct Window* gameWindow);
int create_glfw_window(struct Window* gameWindow, int width, int height);

#endif // WINDOW_H_