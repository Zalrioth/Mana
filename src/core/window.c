#include "core/window.h"
#define STB_IMAGE_IMPLEMENTATION
#include <core/stb_image.h>

//https://stackoverflow.com/questions/2845748/function-defined-but-not-used-warning-in-c

int createWindow(struct Window* window, int width, int height);
int createInstance(struct Window* window);
int setupDebugMessenger(struct Window* window);
int createSurface(struct Window* window);
int pickPhysicalDevice(struct Window* window);
int createLogicalDevice(struct Window* window);
int createSwapChain(struct Window* window);
int createImageViews(struct Window* window);
int createRenderPass(struct Window* window);
int createDescriptorSetLayout(struct Window* window);
int createGraphicsPipeline(struct Window* window);
int createFramebuffers(struct Window* window);
int createTextureImage(struct Window* window);
int createTextureImageView(struct Window* window);
int createTextureSampler(struct Window* window);
int createCommandPool(struct Window* window);
int createDepthResources(struct Window* window);
int createVertexBuffer(struct Window* window);
int createIndexBuffer(struct Window* window);
int createUniformBuffers(struct Window* window);
int createDescriptorPool(struct Window* window);
int createDescriptorSets(struct Window* window);

void assign_vertex(struct Vertex* vertex, float r, float g, float b, float x, float y, float z, float u, float v);

int createCommandBuffers(struct Window* window);
int createSyncObjects(struct Window* window);

bool isDeviceSuitable(struct Window* window, VkPhysicalDevice device);
VkImageView createImageView(struct Window* window, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
int createImage(struct Window* window, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);
VkFormat findDepthFormat(struct Window* window);
int transitionImageLayout(struct Window* window, VkImage* image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
VkCommandBuffer beginSingleTimeCommands();
void endSingleTimeCommands(struct Window* window, VkCommandBuffer commandBuffer);
int createBuffer(struct Window* window, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
void copyBufferToImage(struct Window* window, VkBuffer* buffer, VkImage* image, uint32_t width, uint32_t height);
uint32_t findMemoryType(struct Window* window, uint32_t typeFilter, VkMemoryPropertyFlags properties);
void copyBuffer(struct Window* window, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void cleanupSwapChain(struct Window* window);
VkVertexInputBindingDescription getBindingDescription();
void getAttributeDescriptions(VkVertexInputAttributeDescription* attributeDescriptions);

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{

    fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
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

int init_window(struct Window* window, int width, int height)
{
    int errorCode;

    if ((errorCode = createWindow(window, width, height)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createInstance(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = setupDebugMessenger(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createSurface(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = pickPhysicalDevice(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createLogicalDevice(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createSwapChain(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createImageViews(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createRenderPass(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createDescriptorSetLayout(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createGraphicsPipeline(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createCommandPool(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createDepthResources(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createFramebuffers(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createTextureImage(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createTextureImageView(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createTextureSampler(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createVertexBuffer(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createIndexBuffer(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createUniformBuffers(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createDescriptorPool(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createDescriptorSets(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createCommandBuffers(window)) != NO_ERROR)
        goto cleanup;

    if ((errorCode = createSyncObjects(window)) != NO_ERROR)
        goto cleanup;

    return NO_ERROR;

cleanup:
    delete_window(window);
    return errorCode;
}

void assign_vertex(struct Vertex* vertex, float x, float y, float z, float r, float g, float b, float u, float v)
{
    vertex->pos[0] = x;
    vertex->pos[1] = y;
    vertex->pos[2] = z;

    vertex->color[0] = r;
    vertex->color[1] = g;
    vertex->color[2] = b;

    vertex->texCoord[0] = u;
    vertex->texCoord[1] = v;
}

void delete_window(struct Window* window)
{
    cleanupSwapChain(window);

    //vector_clear(&window->imageVertices);
    //vector_free(&window->imageVertices);

    //vector_clear(&window->imageIndices);
    //vector_free(&window->imageIndices);

    vkDestroySampler(window->device, window->textureSampler, NULL);
    vkDestroyImageView(window->device, window->textureImageView, NULL);

    vkDestroyImage(window->device, window->textureImage, NULL);
    vkFreeMemory(window->device, window->textureImageMemory, NULL);

    vkDestroyDescriptorPool(window->device, window->descriptorPool, NULL);

    vkDestroyDescriptorSetLayout(window->device, window->descriptorSetLayout, NULL);

    for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++) {
        vkDestroyBuffer(window->device, window->uniformBuffers[loopNum], NULL);
        vkFreeMemory(window->device, window->uniformBuffersMemory[loopNum], NULL);
    }

    vkDestroyBuffer(window->device, window->indexBuffer, NULL);
    vkFreeMemory(window->device, window->indexBufferMemory, NULL);

    vkDestroyBuffer(window->device, window->vertexBuffer, NULL);
    vkFreeMemory(window->device, window->vertexBufferMemory, NULL);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(window->device, window->renderFinishedSemaphores[i], NULL);
        vkDestroySemaphore(window->device, window->imageAvailableSemaphores[i], NULL);
        vkDestroyFence(window->device, window->inFlightFences[i], NULL);
    }

    vkDestroyCommandPool(window->device, window->commandPool, NULL);

    vkDestroyDevice(window->device, NULL);

    if (enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(window->instance, window->debugMessenger, NULL);

    vkDestroySurfaceKHR(window->instance, window->surface, NULL);
    vkDestroyInstance(window->instance, NULL);

    glfwDestroyWindow(window->glfwWindow);

    glfwTerminate();
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    struct Window* windowHandle = (struct Window*)(glfwGetWindowUserPointer(window));
    windowHandle->framebufferResized = true;
}

int createWindow(struct Window* window, int width, int height)
{
    memset(window, 0, sizeof(struct Window));

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window->glfwWindow = glfwCreateWindow(width, height, "Grindstone", NULL, NULL);
    window->width = width;
    window->height = height;
    window->framebufferResized = false;
    window->physicalDevice = VK_NULL_HANDLE;
    window->currentFrame = 0;

    glfwSetWindowUserPointer(window->glfwWindow, &window);
    glfwSetFramebufferSizeCallback(window->glfwWindow, framebufferResizeCallback);

    glfwSwapInterval(1);
    glfwMakeContextCurrent(window->glfwWindow);

    if (!window->glfwWindow)
        return CREATE_WINDOW_ERROR;

    assign_vertex(&window->imageVertices[0], -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    assign_vertex(&window->imageVertices[1], 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    assign_vertex(&window->imageVertices[2], 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    assign_vertex(&window->imageVertices[3], -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    /////////////////////////////
    assign_vertex(&window->imageVertices[4], -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    assign_vertex(&window->imageVertices[5], 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    assign_vertex(&window->imageVertices[6], 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    assign_vertex(&window->imageVertices[7], -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    window->imageIndices[0] = 0;
    window->imageIndices[1] = 1;
    window->imageIndices[2] = 2;
    window->imageIndices[3] = 2;
    window->imageIndices[4] = 3;
    window->imageIndices[5] = 0;

    /////////////////////////////
    window->imageIndices[6] = 4;
    window->imageIndices[7] = 5;
    window->imageIndices[8] = 6;
    window->imageIndices[9] = 6;
    window->imageIndices[10] = 7;
    window->imageIndices[11] = 4;

    return NO_ERROR;
}

int createInstance(struct Window* window)
{
    struct VkApplicationInfo appInfo = { 0 };
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Grindstone";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Mana";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    struct VkInstanceCreateInfo createInfo = { 0 };
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    /*uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

    struct VkExtensionProperties extensions[(int)extensionCount];
    memset(extensions, 0, sizeof(extensions));

    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

    printf("Available extensions: ");
    for (int loopNum = 0; loopNum < extensionCount; loopNum++)
        printf("\t %s\n", extensions[loopNum].extensionName);*/

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    const char* convertedExtensions[3];
    memset(convertedExtensions, 0, sizeof(convertedExtensions));

    for (int loopNum = 0; loopNum < glfwExtensionCount; loopNum++)
        convertedExtensions[loopNum] = glfwExtensions[loopNum];

    if (enableValidationLayers) {
        convertedExtensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; //"VK_EXT_DEBUG_UTILS_EXTENSION_NAME\0";
        glfwExtensionCount++;
    }

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = convertedExtensions;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)1;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, NULL, &window->instance) != VK_SUCCESS)
        return CREATE_INSTANCE_ERROR;

    return NO_ERROR;
}
int setupDebugMessenger(struct Window* window)
{
    if (enableValidationLayers && !checkValidationLayerSupport())
        return SETUP_DEBUG_MESSENGER_ERROR;

    if (enableValidationLayers) {
        VkDebugUtilsMessengerCreateInfoEXT debugInfo = { 0 };
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        if (CreateDebugUtilsMessengerEXT(window->instance, &debugInfo, NULL, &window->debugMessenger) != VK_SUCCESS)
            return SETUP_DEBUG_MESSENGER_ERROR;
    }

    return NO_ERROR;
}
int createSurface(struct Window* window)
{
    if (glfwCreateWindowSurface(window->instance, window->glfwWindow, NULL, &window->surface) != VK_SUCCESS)
        return CREATE_SURFACE_ERROR;

    return NO_ERROR;
}
int pickPhysicalDevice(struct Window* window)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(window->instance, &deviceCount, NULL);

    if (deviceCount == 0)
        return PICK_PHYSICAL_DEVICE_ERROR;

    VkPhysicalDevice devices[deviceCount];
    memset(devices, 0, sizeof(devices));

    vkEnumeratePhysicalDevices(window->instance, &deviceCount, devices);

    for (int loopNum = 0; loopNum < deviceCount; loopNum++) {
        if (isDeviceSuitable(window, devices[loopNum])) {
            window->physicalDevice = devices[loopNum];
            break;
        }
    }

    if (window->physicalDevice == VK_NULL_HANDLE)
        return PICK_PHYSICAL_DEVICE_ERROR;

    return NO_ERROR;
}
int createLogicalDevice(struct Window* window)
{
    // Create device
    //TODO: LOOK MORE INTO THIS NOT CORRECT
    int queueCreateInfosSize = 1;
    VkDeviceQueueCreateInfo queueCreateInfos[queueCreateInfosSize];
    memset(queueCreateInfos, 0, sizeof(queueCreateInfos));

    float queuePriority = 1.0f;

    queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[0].queueFamilyIndex = (&window->indices)->graphicsFamily;
    queueCreateInfos[0].queueCount = 1;
    queueCreateInfos[0].pQueuePriorities = &queuePriority;

    //if ((&window->indices)->graphicsFamily != (&window->indices)->presentFamily) {
    //    queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    //    queueCreateInfos[1].queueFamilyIndex = (&window->indices)->presentFamily;
    //    queueCreateInfos[1].queueCount = 1;
    //    queueCreateInfos[1].pQueuePriorities = &queuePriority;
    //}

    struct VkPhysicalDeviceFeatures deviceFeatures = { 0 };
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    struct VkDeviceCreateInfo deviceInfo = { 0 };
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    //if ((&window->indices)->graphicsFamily != (&window->indices)->presentFamily)
    //    deviceInfo.queueCreateInfoCount = (uint32_t)(queueCreateInfosSize);
    //else
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = queueCreateInfos;

    deviceInfo.pEnabledFeatures = &deviceFeatures;

    deviceInfo.enabledExtensionCount = (int32_t)1;
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;

    if (enableValidationLayers) {
        deviceInfo.enabledLayerCount = (uint32_t)1;
        deviceInfo.ppEnabledLayerNames = validationLayers;
    } else {
        deviceInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(window->physicalDevice, &deviceInfo, NULL, &window->device) != VK_SUCCESS)
        return CREATE_LOGICAL_DEVICE_ERROR;

    vkGetDeviceQueue(window->device, window->indices.graphicsFamily, 0, &window->graphicsQueue);
    vkGetDeviceQueue(window->device, window->indices.presentFamily, 0, &window->presentQueue);

    return NO_ERROR;
}

int createSwapChain(struct Window* window)
{
    struct SwapChainSupportDetails swapChainSupport = { { 0 } };

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(window->physicalDevice, window->surface, &swapChainSupport.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(window->physicalDevice, window->surface, &formatCount, NULL);

    if (formatCount != 0)
        vkGetPhysicalDeviceSurfaceFormatsKHR(window->physicalDevice, window->surface, &formatCount, swapChainSupport.formats);

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(window->physicalDevice, window->surface, &presentModeCount, NULL);

    if (presentModeCount != 0)
        vkGetPhysicalDeviceSurfacePresentModesKHR(window->physicalDevice, window->surface, &presentModeCount, swapChainSupport.presentModes);

    VkSurfaceFormatKHR surfaceFormat = { 0 };

    if (formatCount == 1 && swapChainSupport.formats[0].format == VK_FORMAT_UNDEFINED) {
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    } else {
        for (int loopNum = 0; loopNum < SMALL_BUFFER; loopNum++) {
            if (swapChainSupport.formats[loopNum].format == VK_FORMAT_B8G8R8A8_UNORM && swapChainSupport.formats[loopNum].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surfaceFormat = swapChainSupport.formats[loopNum];
                break;
            } else
                surfaceFormat = swapChainSupport.formats[0];
        }
    }

    //https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPresentModeKHR.html
    VkPresentModeKHR presentMode = { 0 };

    for (int loopNum = 0; loopNum < SMALL_BUFFER; loopNum++) {
        if (swapChainSupport.presentModes[loopNum] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = swapChainSupport.presentModes[loopNum];
            break;
        } else if (swapChainSupport.presentModes[loopNum] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            presentMode = swapChainSupport.presentModes[loopNum];
        }
    }

    // Force Vsync
    presentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkExtent2D extent = { window->width, window->height };

    if (swapChainSupport.capabilities.currentExtent.width != UINT32_MAX) {
        extent = swapChainSupport.capabilities.currentExtent;
    } else {
        extent.width = MAX(swapChainSupport.capabilities.minImageExtent.width, MIN(swapChainSupport.capabilities.maxImageExtent.width, extent.width));
        extent.height = MAX(swapChainSupport.capabilities.minImageExtent.height, MIN(swapChainSupport.capabilities.maxImageExtent.height, extent.height));
    }

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchainInfo = { 0 };
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = window->surface;

    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = extent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = { (&window->indices)->graphicsFamily, (&window->indices)->presentFamily };

    if ((&window->indices)->graphicsFamily != (&window->indices)->presentFamily) {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    swapchainInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(window->device, &swapchainInfo, NULL, &window->swapChain) != VK_SUCCESS)
        return CREATE_SWAP_CHAIN_ERROR;

    vkGetSwapchainImagesKHR(window->device, window->swapChain, &imageCount, NULL);
    vkGetSwapchainImagesKHR(window->device, window->swapChain, &imageCount, window->swapChainImages);

    window->swapChainImageFormat = surfaceFormat.format;
    window->swapChainExtent = extent;

    return NO_ERROR;
}

int createImageViews(struct Window* window)
{

    //for (uint32_t i = 0; i < swapChainImages.size(); i++) {
    //        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    //    }
    //}

    for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
        VkImageViewCreateInfo viewInfo = { 0 };
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = window->swapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = window->swapChainImageFormat;
        //createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        //createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        //createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        //createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(window->device, &viewInfo, NULL, &window->swapChainImageViews[i]) != VK_SUCCESS)
            return CREATE_IMAGE_VIEWS_ERROR;
    }

    return NO_ERROR;
}

int createRenderPass(struct Window* window)
{
    VkAttachmentDescription colorAttachment = { 0 };
    colorAttachment.format = window->swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment = { 0 };
    depthAttachment.format = findDepthFormat(window);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = { 0 };
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = { 0 };
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = { 0 };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = { 0 };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[2] = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = { 0 };
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(window->device, &renderPassInfo, NULL, &window->renderPass) != VK_SUCCESS)
        return CREATE_RENDER_PASS_ERROR;

    return NO_ERROR;
}

int createDescriptorSetLayout(struct Window* window)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding = { 0 };
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = NULL;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = { 0 };
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = NULL;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding bindings[2] = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo = { 0 };
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(window->device, &layoutInfo, NULL, &window->descriptorSetLayout) != VK_SUCCESS)
        return -1;
    //printf("failed to create descriptor set layout!");

    return 0;
}

VkShaderModule createShaderModule(struct Window* window, const char* code, int length)
{
    VkShaderModuleCreateInfo createInfo = { 0 };
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = length;
    createInfo.pCode = (const uint32_t*)(code);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(window->device, &createInfo, NULL, &shaderModule) != VK_SUCCESS)
        fprintf(stderr, "failed to create shader module!\n");
    //return 1;

    return shaderModule;
}

char* readShaderFile(const char* filename, int* fileLength)
{
    FILE* fp = fopen(filename, "rb");

    fseek(fp, 0, SEEK_END);
    long int size = ftell(fp);
    rewind(fp);

    *fileLength = size;

    char* result = (char*)malloc(size);

    int index = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        result[index] = c;
        index++;
    }

    fclose(fp);

    return result;
}

int createGraphicsPipeline(struct Window* window)
{
// Get the current working directory
#if defined(IS_WINDOWS)
    char* buffer;
    buffer = _getcwd(NULL, 0);
    printf("%s \nLength: %llu\n", buffer, strlen(buffer));
    free(buffer);
#else
    char cwd[LARGE_BUFFER];
    getcwd(cwd, sizeof(cwd);
    printf("Current working dir: %s\n", cwd);
#endif

    int vertexLength = 0;
    int fragmentLength = 0;

    char* vertShaderCode = readShaderFile("./Assets/shaders/textureVert.spv", &vertexLength);
    char* fragShaderCode = readShaderFile("./Assets/shaders/textureFrag.spv", &fragmentLength);

    VkShaderModule vertShaderModule = createShaderModule(window, vertShaderCode, vertexLength);
    VkShaderModule fragShaderModule = createShaderModule(window, fragShaderCode, fragmentLength);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = { 0 };
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = { 0 };
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = { 0 };
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkVertexInputBindingDescription bindingDescription = getBindingDescription();
    VkVertexInputAttributeDescription attributeDescriptions[3];
    memset(attributeDescriptions, 0, sizeof(attributeDescriptions));
    getAttributeDescriptions(attributeDescriptions);

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = 3; // Note: length of attributeDescriptions
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = { 0 };
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = { 0 };
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)window->swapChainExtent.width;
    viewport.height = (float)window->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { 0 };
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = window->swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = { 0 };
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil = { 0 };
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = { 0 };
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = { 0 };
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &window->descriptorSetLayout;

    if (vkCreatePipelineLayout(window->device, &pipelineLayoutInfo, NULL, &window->pipelineLayout) != VK_SUCCESS)
        return CREATE_GRAPHICS_PIPELINE_ERROR;

    VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = window->pipelineLayout;
    pipelineInfo.renderPass = window->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(window->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &window->graphicsPipeline) != VK_SUCCESS)
        return CREATE_GRAPHICS_PIPELINE_ERROR;

    vkDestroyShaderModule(window->device, fragShaderModule, NULL);
    vkDestroyShaderModule(window->device, vertShaderModule, NULL);

    return NO_ERROR;
}

int createFramebuffers(struct Window* window)
{
    for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++) {
        VkImageView attachments[] = { window->swapChainImageViews[loopNum], window->depthImageView };

        VkFramebufferCreateInfo framebufferInfo = { 0 };
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = window->renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = window->swapChainExtent.width;
        framebufferInfo.height = window->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(window->device, &framebufferInfo, NULL, &window->swapChainFramebuffers[loopNum]) != VK_SUCCESS)
            return CREATE_FRAME_BUFFER_ERROR;
    }

    return NO_ERROR;
}

int createTextureImage(struct Window* window)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("./Assets/textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        return -1;
    //printf("failed to load texture image!\n");

    VkBuffer stagingBuffer = { 0 };
    VkDeviceMemory stagingBufferMemory = { 0 };
    createBuffer(window, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(window->device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(window->device, stagingBufferMemory);

    stbi_image_free(pixels);

    createImage(window, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &window->textureImage, &window->textureImageMemory);

    transitionImageLayout(window, &window->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(window, &stagingBuffer, &window->textureImage, texWidth, texHeight);
    transitionImageLayout(window, &window->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(window->device, stagingBuffer, NULL);
    vkFreeMemory(window->device, stagingBufferMemory, NULL);

    return 0;
}

int createTextureImageView(struct Window* window)
{
    window->textureImageView = createImageView(window, window->textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

    return 0;
}

int createTextureSampler(struct Window* window)
{
    VkSamplerCreateInfo samplerInfo = { 0 };
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(window->device, &samplerInfo, NULL, &window->textureSampler) != VK_SUCCESS)
        return -1;
    //printf("failed to create texture sampler!\n");

    return 0;
}

int createCommandPool(struct Window* window)
{
    VkCommandPoolCreateInfo poolInfo = { 0 };
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = window->indices.graphicsFamily;

    if (vkCreateCommandPool(window->device, &poolInfo, NULL, &window->commandPool) != VK_SUCCESS)
        return CREATE_COMMAND_POOL_ERROR;

    return NO_ERROR;
}

int createDepthResources(struct Window* window)
{
    VkFormat depthFormat = findDepthFormat(window);

    createImage(window, window->swapChainExtent.width, window->swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &window->depthImage, &window->depthImageMemory);
    window->depthImageView = createImageView(window, window->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    transitionImageLayout(window, &window->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    return 0;
}

int createCommandBuffers(struct Window* window)
{
    VkCommandBufferAllocateInfo allocInfo = { 0 };
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = window->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)MAX_SWAP_CHAIN_FRAMES;

    if (vkAllocateCommandBuffers(window->device, &allocInfo, window->commandBuffers) != VK_SUCCESS)
        return CREATE_COMMAND_BUFFER_ERROR;

    for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
        VkCommandBufferBeginInfo beginInfo = { 0 };
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        if (vkBeginCommandBuffer(window->commandBuffers[i], &beginInfo) != VK_SUCCESS)
            return CREATE_COMMAND_BUFFER_ERROR;

        VkRenderPassBeginInfo renderPassInfo = { 0 };
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = window->renderPass;
        renderPassInfo.framebuffer = window->swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset.x = 0;
        renderPassInfo.renderArea.offset.y = 0;
        renderPassInfo.renderArea.extent = window->swapChainExtent;

        VkClearValue clearValues[2];
        memset(clearValues, 0, sizeof(clearValues));

        //http://ogldev.atspace.co.uk/www/tutorial51/tutorial51.html
        VkClearColorValue clearColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        clearValues[0].color = clearColor;

        VkClearDepthStencilValue depthColor = { 1.0f, 0 };
        clearValues[1].depthStencil = depthColor;

        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(window->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(window->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, window->graphicsPipeline);

        VkBuffer vertexBuffers[] = { window->vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(window->commandBuffers[i], 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(window->commandBuffers[i], window->indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(window->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, window->pipelineLayout, 0, 1, &window->descriptorSets[i], 0, NULL);

        //vkCmdDrawIndexed(window->commandBuffers[i], window->imageIndices.total, 1, 0, 0, 0);
        vkCmdDrawIndexed(window->commandBuffers[i], 12, 1, 0, 0, 0);
        vkCmdEndRenderPass(window->commandBuffers[i]);

        if (vkEndCommandBuffer(window->commandBuffers[i]) != VK_SUCCESS)
            return CREATE_COMMAND_BUFFER_ERROR;
    }

    return NO_ERROR;
}

int createSyncObjects(struct Window* window)
{
    memset(window->inFlightFences, 0, sizeof(window->inFlightFences));

    VkSemaphoreCreateInfo semaphoreInfo = { 0 };
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = { 0 };
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(window->device, &semaphoreInfo, NULL, &window->imageAvailableSemaphores[i]) != VK_SUCCESS || vkCreateSemaphore(window->device, &semaphoreInfo, NULL, &window->renderFinishedSemaphores[i]) != VK_SUCCESS || vkCreateFence(window->device, &fenceInfo, NULL, &window->inFlightFences[i]) != VK_SUCCESS)
            return CREATE_SYNC_OBJECT_ERROR;
    }

    return NO_ERROR;
}

bool isDeviceSuitable(struct Window* window, VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    struct VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    memset(queueFamilies, 0, sizeof(queueFamilies));

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    bool graphicsFamilyFound = false;
    bool presentFamilyFound = false;

    int i = 0;
    for (int loopNum = 0; loopNum < queueFamilyCount; loopNum++) {
        if (queueFamilies[loopNum].queueCount > 0 && queueFamilies[loopNum].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            (&window->indices)->graphicsFamily = i;
            graphicsFamilyFound = true;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, window->surface, &presentSupport);

        if (queueFamilies[loopNum].queueCount > 0 && presentSupport) {
            (&window->indices)->presentFamily = i;
            presentFamilyFound = true;
        }

        if (graphicsFamilyFound && presentFamilyFound)
            break;

        i++;
    }

    if (!graphicsFamilyFound || !presentFamilyFound)
        return false;

    return true;
}

bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties availableLayers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    bool layerFound = false;

    for (int loopNum = 0; loopNum < layerCount; loopNum++) {
        if (strcmp(validationLayers[0], availableLayers[loopNum].layerName) == 0) {
            layerFound = true;
            break;
        }
    }

    return layerFound;
}

VkImageView createImageView(struct Window* window, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo = { 0 };
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(window->device, &viewInfo, NULL, &imageView) != VK_SUCCESS)
        fprintf(stderr, "failed to create texture image view!");

    return imageView;
}
#define TOTAL_CANDIDIATES 3
VkFormat findDepthFormat(struct Window* window)
{
    VkFormat candidate[TOTAL_CANDIDIATES] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (int loopNum = 0; loopNum < TOTAL_CANDIDIATES; loopNum++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(window->physicalDevice, candidate[loopNum], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return candidate[loopNum];
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return candidate[loopNum];
        }
    }

    //printf("failed to find supported format!");

    //return 0;
    return -1;
}

int createImage(struct Window* window, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory)
{
    VkImageCreateInfo imageInfo = { 0 };
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(window->device, &imageInfo, NULL, image) != VK_SUCCESS)
        return -1;
    //printf("failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(window->device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = { 0 };
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(window, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(window->device, &allocInfo, NULL, imageMemory) != VK_SUCCESS)
        return -1;
    //printf("failed to allocate image memory!");

    vkBindImageMemory(window->device, *image, *imageMemory, 0);

    return 0;
}

int transitionImageLayout(struct Window* window, VkImage* image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = { 0 };
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = *image;

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    } else
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage = { 0 };
    VkPipelineStageFlags destinationStage = { 0 };

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        fprintf(stderr, "unsupported layout transition!");
        return -1;
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier);

    endSingleTimeCommands(window, commandBuffer);

    return 0;
}

VkCommandBuffer beginSingleTimeCommands(struct Window* window)
{
    VkCommandBufferAllocateInfo allocInfo = { 0 };
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = window->commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(window->device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = { 0 };
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void endSingleTimeCommands(struct Window* window, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = { 0 };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(window->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(window->graphicsQueue);

    vkFreeCommandBuffers(window->device, window->commandPool, 1, &commandBuffer);
}

int createBuffer(struct Window* window, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
    VkBufferCreateInfo bufferInfo = { 0 };
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(window->device, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        return -1;
        fprintf(stderr, "failed to create buffer!\n");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(window->device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = { 0 };
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(window, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(window->device, &allocInfo, NULL, bufferMemory) != VK_SUCCESS) {
        return -1;
        fprintf(stderr, "failed to allocate buffer memory!\n");
    }

    vkBindBufferMemory(window->device, *buffer, *bufferMemory, 0);

    return 0;
}

void copyBufferToImage(struct Window* window, VkBuffer* buffer, VkImage* image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(window);

    VkBufferImageCopy region = { 0 };
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;
    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(commandBuffer, *buffer, *image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(window, commandBuffer);
}

uint32_t findMemoryType(struct Window* window, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(window->physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    printf("failed to find suitable memory type!\n");

    return -1;
}

int createVertexBuffer(struct Window* window)
{
    VkDeviceSize bufferSize = sizeof(struct Vertex) * 8; //window->imageVertices.total;
    //VkDeviceSize bufferSize = sizeof(window->imageVertices.items[0]) * window->imageVertices.total;

    VkBuffer stagingBuffer = { 0 };
    VkDeviceMemory stagingBufferMemory = { 0 };
    createBuffer(window, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(window->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, window->imageVertices, bufferSize);
    vkUnmapMemory(window->device, stagingBufferMemory);

    createBuffer(window, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &window->vertexBuffer, &window->vertexBufferMemory);

    copyBuffer(window, stagingBuffer, window->vertexBuffer, bufferSize);

    vkDestroyBuffer(window->device, stagingBuffer, NULL);
    vkFreeMemory(window->device, stagingBufferMemory, NULL);

    return 0;
}

void copyBuffer(struct Window* window, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(window);

    VkBufferCopy copyRegion = { 0 };
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(window, commandBuffer);
}

int createIndexBuffer(struct Window* window)
{
    VkDeviceSize bufferSize = sizeof(uint16_t) * 12; //window->imageIndices.total;
    //VkDeviceSize bufferSize = sizeof(window->imageIndices.items[0]) * window->imageIndices.total;

    VkBuffer stagingBuffer = { 0 };
    VkDeviceMemory stagingBufferMemory = { 0 };
    createBuffer(window, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(window->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, window->imageIndices, bufferSize);
    vkUnmapMemory(window->device, stagingBufferMemory);

    createBuffer(window, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &window->indexBuffer, &window->indexBufferMemory);

    copyBuffer(window, stagingBuffer, window->indexBuffer, bufferSize);

    vkDestroyBuffer(window->device, stagingBuffer, NULL);
    vkFreeMemory(window->device, stagingBufferMemory, NULL);

    return 0;
}

int createUniformBuffers(struct Window* window)
{
    VkDeviceSize bufferSize = sizeof(struct UniformBufferObject);

    for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++)
        createBuffer(window, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &window->uniformBuffers[i], &window->uniformBuffersMemory[i]);

    return 0;
}

int createDescriptorPool(struct Window* window)
{
    VkDescriptorPoolSize poolSizes[2];
    memset(poolSizes, 0, sizeof(poolSizes));

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = MAX_SWAP_CHAIN_FRAMES;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = MAX_SWAP_CHAIN_FRAMES;

    VkDescriptorPoolCreateInfo poolInfo = { 0 };
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = MAX_SWAP_CHAIN_FRAMES;

    if (vkCreateDescriptorPool(window->device, &poolInfo, NULL, &window->descriptorPool) != VK_SUCCESS) {
        fprintf(stderr, "failed to create descriptor pool!\n");
        return -1;
    }

    return 0;
}

int createDescriptorSets(struct Window* window)
{
    VkDescriptorSetLayout layouts[MAX_SWAP_CHAIN_FRAMES];
    memset(layouts, 0, sizeof(layouts));

    for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
        layouts[loopNum] = window->descriptorSetLayout;

    VkDescriptorSetAllocateInfo allocInfo = { 0 };
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = window->descriptorPool;
    allocInfo.descriptorSetCount = MAX_SWAP_CHAIN_FRAMES;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(window->device, &allocInfo, window->descriptorSets) != VK_SUCCESS) {
        fprintf(stderr, "failed to allocate descriptor sets!\n");
        return -1;
    }

    for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
        VkDescriptorBufferInfo bufferInfo = { 0 };
        bufferInfo.buffer = window->uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(struct UniformBufferObject);

        VkDescriptorImageInfo imageInfo = { 0 };
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = window->textureImageView;
        imageInfo.sampler = window->textureSampler;

        int descriptorSize = 2;
        VkWriteDescriptorSet descriptorWrites[descriptorSize];
        memset(descriptorWrites, 0, sizeof(descriptorWrites));

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = window->descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = window->descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(window->device, descriptorSize, descriptorWrites, 0, NULL);
    }

    return 0;
}

void recreateSwapChain(struct Window* window)
{
    int width = 0, height = 0;
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->glfwWindow, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(window->device);

    cleanupSwapChain(window);

    createSwapChain(window);
    createImageViews(window);
    createRenderPass(window);
    createGraphicsPipeline(window);
    createDepthResources(window);
    createFramebuffers(window);
    createCommandBuffers(window);
}

void cleanupSwapChain(struct Window* window)
{
    vkDestroyImageView(window->device, window->depthImageView, NULL);
    vkDestroyImage(window->device, window->depthImage, NULL);
    vkFreeMemory(window->device, window->depthImageMemory, NULL);

    for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
        vkDestroyFramebuffer(window->device, window->swapChainFramebuffers[loopNum], NULL);

    vkFreeCommandBuffers(window->device, window->commandPool, SMALL_BUFFER, window->commandBuffers);

    vkDestroyPipeline(window->device, window->graphicsPipeline, NULL);
    vkDestroyPipelineLayout(window->device, window->pipelineLayout, NULL);
    vkDestroyRenderPass(window->device, window->renderPass, NULL);

    for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
        vkDestroyImageView(window->device, window->swapChainImageViews[loopNum], NULL);

    vkDestroySwapchainKHR(window->device, window->swapChain, NULL);
}

VkVertexInputBindingDescription getBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = { 0 };
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(struct Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

void getAttributeDescriptions(VkVertexInputAttributeDescription* attributeDescriptions)
{
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(struct Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(struct Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(struct Vertex, texCoord);
}
