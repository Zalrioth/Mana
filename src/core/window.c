#include "core/window.h"

bool isDeviceSuitable(struct Window* window, VkPhysicalDevice device);

int init_window(struct Window* window)
{
    if (!glfwInit())
        return 1;

    if (!glfwVulkanSupported())
        return 2;

    return 0;
}

void delete_window(struct Window* window)
{
    vkDestroyDevice(window->device, NULL);

    //if (enableValidationLayers) {
    //    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    //}

    vkDestroySurfaceKHR(window->instance, window->surface, NULL);
    vkDestroyInstance(window->instance, NULL);

    glfwDestroyWindow(window->glfwWindow);

    glfwTerminate();
}

int create_glfw_window(struct Window* window, int width, int height)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window->glfwWindow = glfwCreateWindow(width, height, "Grindstone", NULL, NULL);
    window->width = width;
    window->height = height;

    if (!window->glfwWindow)
        return 1;

    glfwSwapInterval(1);
    glfwMakeContextCurrent(window->glfwWindow);

    window->physicalDevice = VK_NULL_HANDLE;

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

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, NULL, &window->instance) != VK_SUCCESS)
        return 2;

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

    struct VkExtensionProperties extensions[(int)extensionCount];
    memset(extensions, 0, sizeof(extensions));

    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

    printf("Available extensions: ");
    for (int loopNum = 0; loopNum < extensionCount; loopNum++)
        printf("\t %s\n", extensions[loopNum].extensionName);

    if (glfwCreateWindowSurface(window->instance, window->glfwWindow, NULL, &window->surface) != VK_SUCCESS)
        return 3;

    // Pick Device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(window->instance, &deviceCount, NULL);

    if (deviceCount == 0)
        return 4;

    VkPhysicalDevice devices[(int)deviceCount];
    memset(devices, 0, sizeof(devices));

    vkEnumeratePhysicalDevices(window->instance, &deviceCount, devices);

    for (int loopNum = 0; loopNum < deviceCount; loopNum++) {
        if (isDeviceSuitable(window, devices[loopNum])) {
            window->physicalDevice = devices[loopNum];
            break;
        }
    }

    if (window->physicalDevice == VK_NULL_HANDLE)
        return 5;

    // Create device
    int queueCreateInfosSize = 2;
    VkDeviceQueueCreateInfo queueCreateInfos[(int)queueCreateInfosSize];
    memset(queueCreateInfos, 0, sizeof(queueCreateInfos));

    //std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    //std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
    //
    float queuePriority = 1.0f;

    queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[0].queueFamilyIndex = (&window->indices)->graphicsFamily;
    queueCreateInfos[0].queueCount = 1;
    queueCreateInfos[0].pQueuePriorities = &queuePriority;

    queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[1].queueFamilyIndex = (&window->indices)->presentFamily;
    queueCreateInfos[1].queueCount = 1;
    queueCreateInfos[1].pQueuePriorities = &queuePriority;

    struct VkPhysicalDeviceFeatures deviceFeatures = { 0 };

    struct VkDeviceCreateInfo createInfo2 = { 0 };
    createInfo2.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo2.queueCreateInfoCount = (uint32_t)(queueCreateInfosSize);
    createInfo2.pQueueCreateInfos = queueCreateInfos;
    createInfo2.pEnabledFeatures = &deviceFeatures;
    createInfo2.enabledExtensionCount = 0;

    /*if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }*/

    if (vkCreateDevice(window->physicalDevice, &createInfo2, NULL, &window->device) != VK_SUCCESS)
        return 6;

    vkGetDeviceQueue(window->device, window->indices.graphicsFamily, 0, &window->graphicsQueue);
    vkGetDeviceQueue(window->device, window->indices.presentFamily, 0, &window->presentQueue);
    //glfwSetWindowPos(&window->instance, 0, 0);
}

bool isDeviceSuitable(struct Window* window, VkPhysicalDevice device)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    struct VkQueueFamilyProperties queueFamilies[(int)queueFamilyCount];
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

    return graphicsFamilyFound && presentFamilyFound;
}
