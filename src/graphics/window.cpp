#include "graphics/window.hpp"

Window::Window(bool vSync, int width, int height)
{
    if (!glfwInit())
        throw std::runtime_error("Unable to start GLFW");

    //    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //#ifdef __APPLE__
    //    glfwWindowHint(
    //        GLFW_OPENGL_FORWARD_COMPAT,
    //        GL_TRUE); // uncomment this statement to fix compilation on OS X
    //#endif

    if (glfwVulkanSupported())
        std::cout << "Vulkan support found" << std::endl;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Grindstone", NULL, NULL);

    if (!window)
        throw std::runtime_error("Unable to create Vulkan window");

    glfwMakeContextCurrent(window);
    glfwSwapInterval(vSync); // vsync 0 = false, 1 = true
    //glewExperimental = GL_TRUE; // start GLEW extension handler
    //glewInit();

    //const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    //const GLubyte* version = glGetString(GL_VERSION); // version as a string
    //std::cout << "Renderer: " << renderer << std::endl;
    //std::cout << "OpenGL version supported " << version << std::endl;

    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);
    //glEnable(GL_BLEND); // Support for transparencies
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glfwWindowHint(GLFW_SAMPLES, 4); // Antialiasing

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "available extensions:" << std::endl;

    for (const auto& extension : extensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
    }

    this->glWindow = window;
    this->vSync = vSync;
    this->limitFPS = false;
    this->width = width;
    this->height = height;
}

Window::~Window()
{
    vkDestroyInstance(this->instance, nullptr);

    glfwDestroyWindow(this->glWindow);

    glfwTerminate(); // close GL context and any other GLFW resources
}