#include "core/window.h"

void init_window(struct Window* window)
{
    if (!glfwInit()) {
        printf("Error initializing GLFW!\n");
        exit(1);
    }

    if (!glfwVulkanSupported()) {
        printf("Vulkan support not found!\n");
        exit(1);
    }
}

void delete_window(struct Window* window)
{
}

void create_glfw_window(struct Window* window)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window->glfwWindow = glfwCreateWindow(640, 480, "Grindstone", NULL, NULL);

    if (!window->glfwWindow) {
        glfwTerminate();
        printf("Error creating GLFW window!\n");
        exit(1);
    }

    glfwSwapInterval(1);
    glfwMakeContextCurrent(window->glfwWindow);

    //VkApplicationInfo appInfo = {};
    //appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    //appInfo.pApplicationName = "Hello Triangle";
    //appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    //appInfo.pEngineName = "No Engine";
    //appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    //appInfo.apiVersion = VK_API_VERSION_1_0;
    //
    //VkInstanceCreateInfo createInfo = {};
    //createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    //createInfo.pApplicationInfo = &appInfo;
    //
    //uint32_t glfwExtensionCount = 0;
    //const char** glfwExtensions;
    //
    //glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //
    //createInfo.enabledExtensionCount = glfwExtensionCount;
    //createInfo.ppEnabledExtensionNames = glfwExtensions;
    //
    //createInfo.enabledLayerCount = 0;
    //
    //VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    //
    //if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    //    throw std::runtime_error("failed to create instance!");

    //uint32_t extensionCount = 0;
    //vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    //
    //std::vector<VkExtensionProperties> extensions(extensionCount);
    //
    //vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    //
    //std::cout << "available extensions:" << std::endl;
    //
    //for (const auto& extension : extensions) {
    //    std::cout << "\t" << extension.extensionName << std::endl;
    //}
}
