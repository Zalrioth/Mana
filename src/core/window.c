#include "core/window.h"

int createSwapChain(struct Window* window);
int createImageViews(struct Window* window);
int createRenderPass(struct Window* window);
int createGraphicsPipeline(struct Window* window);
bool isDeviceSuitable(struct Window* window, VkPhysicalDevice device);

int init_window(struct Window* window)
{
    if (!glfwInit())
        return 1;

    if (!glfwVulkanSupported())
        return 2;

    memset(window, 0, sizeof(struct Window));

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

    if (enableValidationLayers && !checkValidationLayerSupport())
        return 8; // TODO: Update return
    //throw std::runtime_error("validation layers requested, but not available!");

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

    const char* convertedExtensions[3];
    memset(convertedExtensions, 0, sizeof(convertedExtensions));

    for (int loopNum = 0; loopNum < glfwExtensionCount; loopNum++)
        convertedExtensions[loopNum] = glfwExtensions[loopNum];

    if (enableValidationLayers) {
        convertedExtensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; //"VK_EXT_DEBUG_UTILS_EXTENSION_NAME\0";
        glfwExtensionCount++;
        //extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = convertedExtensions; //glfwExtensions;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)1;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, NULL, &window->instance) != VK_SUCCESS)
        return 2;

    if (enableValidationLayers) {
        VkDebugUtilsMessengerCreateInfoEXT debugInfo = { 0 };
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        if (CreateDebugUtilsMessengerEXT(window->instance, &debugInfo, NULL, &window->debugMessenger) != VK_SUCCESS) {
            return 9;
            //throw std::runtime_error("failed to set up debug messenger!");
            //TODO: Add return
        }
    }

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

    struct VkDeviceCreateInfo deviceInfo = { 0 };
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceInfo.queueCreateInfoCount = (uint32_t)(queueCreateInfosSize);
    deviceInfo.pQueueCreateInfos = queueCreateInfos;

    deviceInfo.pEnabledFeatures = &deviceFeatures;

    deviceInfo.enabledExtensionCount = (int32_t)1;
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;
    //createInfo2.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        deviceInfo.enabledLayerCount = (uint32_t)1;
        deviceInfo.ppEnabledLayerNames = validationLayers;
    } else {
        deviceInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(window->physicalDevice, &deviceInfo, NULL, &window->device) != VK_SUCCESS)
        return 6;

    vkGetDeviceQueue(window->device, window->indices.graphicsFamily, 0, &window->graphicsQueue);
    vkGetDeviceQueue(window->device, window->indices.presentFamily, 0, &window->presentQueue);
    //glfwSetWindowPos(&window->instance, 0, 0);

    int swapChainError = createSwapChain(window);
    if (swapChainError != 0)
        return swapChainError;

    int createImageViewsError = createImageViews(window);
    if (createImageViewsError != 0)
        return createImageViewsError;

    int createRenderPassError = createRenderPass(window);
    if (createRenderPassError != 0)
        return createRenderPassError;

    int createGraphicsPipelineError = createGraphicsPipeline(window);
    if (createGraphicsPipelineError != 0)
        return createGraphicsPipelineError;

    return 0;
}

int createSwapChain(struct Window* window)
{
    struct SwapChainSupportDetails swapChainSupport = { { 0 } };
    //memset(extensions, 0, sizeof(extensions));

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(window->physicalDevice, window->surface, &swapChainSupport.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(window->physicalDevice, window->surface, &formatCount, NULL);

    if (formatCount != 0) {
        //swapChainSupport.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(window->physicalDevice, window->surface, &formatCount, swapChainSupport.formats);
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(window->physicalDevice, window->surface, &presentModeCount, NULL);

    if (presentModeCount != 0) {
        //swapChainSupport.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(window->physicalDevice, window->surface, &presentModeCount, swapChainSupport.presentModes);
    }

    // Start 1

    VkSurfaceFormatKHR surfaceFormat = { 0 };
    //swapChainSupport.formats

    //if (swapChainSupport.formats.availableFormats.size() == 1 && swapChainSupport.formats.availableFormats[0].format == VK_FORMAT_UNDEFINED) {
    if (formatCount == 1 && swapChainSupport.formats[0].format == VK_FORMAT_UNDEFINED) {
        surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
        surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    } else {
        //for (const auto& availableFormat : swapChainSupport.formats) {
        for (int loopNum = 0; loopNum < SMALL_BUFFER; loopNum++) {
            if (swapChainSupport.formats[loopNum].format == VK_FORMAT_B8G8R8A8_UNORM && swapChainSupport.formats[loopNum].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surfaceFormat = swapChainSupport.formats[loopNum];
                break;
            } else
                surfaceFormat = swapChainSupport.formats[0];
        }
    }

    // chooseSwapSurfaceFormat(swapChainSupport.formats);

    // Start 2

    VkPresentModeKHR presentMode = { 0 };
    //VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    //swapChainSupport.presentModes

    //for (const auto& availablePresentMode : availablePresentModes) {
    for (int loopNum = 0; loopNum < SMALL_BUFFER; loopNum++) {
        if (swapChainSupport.presentModes[loopNum] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = swapChainSupport.presentModes[loopNum];
            break;
        } else if (swapChainSupport.presentModes[loopNum] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            presentMode = swapChainSupport.presentModes[loopNum];
        }
    }

    // chooseSwapPresentMode(swapChainSupport.presentModes);

    // Start 3

    VkExtent2D extent = { window->width, window->height };

    if (swapChainSupport.capabilities.currentExtent.width != UINT32_MAX) {
        extent = swapChainSupport.capabilities.currentExtent;
    } else {
        extent.width = MAX(swapChainSupport.capabilities.minImageExtent.width, MIN(swapChainSupport.capabilities.maxImageExtent.width, extent.width));
        extent.height = MAX(swapChainSupport.capabilities.minImageExtent.height, MIN(swapChainSupport.capabilities.maxImageExtent.height, extent.height));
    }

    //chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;
    // End of querySwapChainSupport

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

    //window->swapChain = { 0 };
    if (vkCreateSwapchainKHR(window->device, &swapchainInfo, NULL, &window->swapChain) != VK_SUCCESS)
        return 7;

    vkGetSwapchainImagesKHR(window->device, window->swapChain, &imageCount, NULL);
    //window->swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(window->device, window->swapChain, &imageCount, window->swapChainImages);

    window->swapChainImageFormat = surfaceFormat.format;
    window->swapChainExtent = extent;

    return 0;
}

int createImageViews(struct Window* window)
{
    //swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < 3; i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = window->swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = window->swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(window->device, &createInfo, NULL, &window->swapChainImageViews[i]) != VK_SUCCESS)
            return 8;
        //throw std::runtime_error("failed to create image views!");
    }

    return 0;
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

    VkAttachmentReference colorAttachmentRef = { 0 };
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = { 0 };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = { 0 };
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(window->device, &renderPassInfo, NULL, &window->renderPass) != VK_SUCCESS)
        return 10;
    //throw std::runtime_error("failed to create render pass!");

    return 0;
}

VkShaderModule createShaderModule(struct Window* window, const char* code, int length)
{
    VkShaderModuleCreateInfo createInfo = { 0 };
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = length; //HUGE_BUFFER; //strlen(code);
    createInfo.pCode = (const uint32_t*)(code);

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(window->device, &createInfo, NULL, &shaderModule) != VK_SUCCESS)
        printf("failed to create shader module!\n");
    //return 1;
    //throw std::runtime_error("failed to create shader module!");

    return shaderModule;
}

char* readShaderFile(const char* filename, int* fileLength)
{
    //FILE* ptr = fopen(filename, "r"); // r for read, b for binary
    //fread(text, sizeof(LARGE_BUFFER), 1, ptr);
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
// Get the current working directory:
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

    //char* vertShaderCode = readFile("./assets/shaders/basicVert.spv");
    //char* fragShaderCode = readFile("./assets/shaders/basicFrag.spv");

    //int vertShaderCode[HUGE_BUFFER];
    //int fragShaderCode[HUGE_BUFFER];

    int vertexLength = 0;
    int fragmentLength = 0;

    //memset(vertShaderCode, 0, sizeof(vertShaderCode));
    //memset(fragShaderCode, 0, sizeof(fragShaderCode));

    //char* vertShaderCode = NULL;
    //char* fragShaderCode = NULL;

    char* vertShaderCode = readShaderFile("./assets/shaders/basicVert.spv", &vertexLength);
    char* fragShaderCode = readShaderFile("./assets/shaders/basicFrag.spv", &fragmentLength);

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
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

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
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

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
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(window->device, &pipelineLayoutInfo, NULL, &window->pipelineLayout) != VK_SUCCESS)
        return 11;
    //throw std::runtime_error("failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = window->pipelineLayout;
    pipelineInfo.renderPass = window->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(window->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &window->graphicsPipeline) != VK_SUCCESS)
        return 11;
    //throw std::runtime_error("failed to create graphics pipeline!");

    vkDestroyShaderModule(window->device, fragShaderModule, NULL);
    vkDestroyShaderModule(window->device, vertShaderModule, NULL);

    return 0;
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

    if (!graphicsFamilyFound || !presentFamilyFound)
        return false;

    //return graphicsFamilyFound && presentFamilyFound;

    return true;

    /*bool extensionsSupported = checkDeviceExtensionSupport(device);

    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

        VkExtensionProperties availableExtensions[extensionCount];
        vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return extensionsSupported && swapChainAdequate;*/
}

bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties availableLayers[(int)layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    bool layerFound = false;

    //for (const auto& layerProperties : availableLayers) {
    for (int loopNum = 0; loopNum < layerCount; loopNum++) {
        if (strcmp(validationLayers[0], availableLayers[loopNum].layerName) == 0) {
            layerFound = true;
            break;
        }
    }

    return layerFound;
}
