#include "core/vulkanrenderer.h"

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  struct VulkanRenderer* vulkan_renderer_handle = (struct VulkanRenderer*)(glfwGetWindowUserPointer(window));
  vulkan_renderer_handle->framebuffer_resized = true;
}

int init_vulkan_renderer(struct VulkanRenderer* vulkan_renderer, int width, int height) {
  memset(vulkan_renderer, 0, sizeof(struct VulkanRenderer));

  vulkan_renderer->image_mesh = calloc(1, sizeof(struct Mesh));
  mesh_init(vulkan_renderer->image_mesh);

  vulkan_renderer->image_texture = calloc(1, sizeof(struct Texture));
  texture_init(vulkan_renderer->image_texture, "./Assets/textures/texture.jpg");

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  vulkan_renderer->glfw_window = glfwCreateWindow(width, height, "Grindstone", NULL, NULL);
  vulkan_renderer->framebuffer_resized = false;
  vulkan_renderer->physical_device = VK_NULL_HANDLE;
  vulkan_renderer->current_frame = 0;

  glfwSetWindowUserPointer(vulkan_renderer->glfw_window, &vulkan_renderer);
  glfwSetFramebufferSizeCallback(vulkan_renderer->glfw_window, framebuffer_resize_callback);

  glfwSwapInterval(1);
  glfwMakeContextCurrent(vulkan_renderer->glfw_window);

  if (!vulkan_renderer->glfw_window)
    return CREATE_WINDOW_ERROR;

  mesh_init(vulkan_renderer->image_mesh);

  mesh_assign_vertex(vulkan_renderer->image_mesh->vertices, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  mesh_assign_vertex(vulkan_renderer->image_mesh->vertices, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
  mesh_assign_vertex(vulkan_renderer->image_mesh->vertices, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  mesh_assign_vertex(vulkan_renderer->image_mesh->vertices, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

  mesh_assign_vertex(vulkan_renderer->image_mesh->vertices, -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  mesh_assign_vertex(vulkan_renderer->image_mesh->vertices, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
  mesh_assign_vertex(vulkan_renderer->image_mesh->vertices, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
  mesh_assign_vertex(vulkan_renderer->image_mesh->vertices, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 0);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 1);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 2);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 2);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 3);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 0);

  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 4);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 5);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 6);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 6);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 7);
  mesh_assign_indice(vulkan_renderer->image_mesh->indices, 4);

  return NO_ERROR;
}

void vulkan_renderer_delete(struct VulkanRenderer* vulkan_renderer) {
  vulkan_swap_chain_cleanup(vulkan_renderer);
  vulkan_texture_cleanup(vulkan_renderer);
  vulkan_descriptor_set_layout_cleanup(vulkan_renderer);
  vulkan_index_buffer_cleanup(vulkan_renderer);
  vulkan_vertex_buffer_cleanup(vulkan_renderer);
  vulkan_sync_objects_cleanup(vulkan_renderer);
  vulkan_command_pool_cleanup(vulkan_renderer);

  //
  for (int buffer_num = 0; buffer_num < MAX_SWAP_CHAIN_FRAMES; buffer_num++) {
    vkDestroyBuffer(vulkan_renderer->device, vulkan_renderer->uniform_buffers[buffer_num], NULL);
    vkFreeMemory(vulkan_renderer->device, vulkan_renderer->uniform_buffers_memory[buffer_num], NULL);
  }
  //

  vulkan_device_cleanup(vulkan_renderer);
  vulkan_debug_cleanup(vulkan_renderer);
  vulkan_surface_cleanup(vulkan_renderer);
  window_cleanup(vulkan_renderer);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_uiser_data) {
  fprintf(stderr, "validation layer: %s\n", p_callback_data->pMessage);
  return VK_FALSE;
}

static VkResult create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_create_info, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debug_messenger) {
  PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != NULL)
    return func(instance, p_create_info, p_allocator, p_debug_messenger);
  else
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* p_allocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL)
    func(instance, debug_messenger, p_allocator);
}

void copy_buffer(struct VulkanRenderer* vulkan_renderer, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
  VkCommandBuffer command_buffer = graphics_utils_begin_single_time_commands(vulkan_renderer->device, vulkan_renderer->command_pool);

  VkBufferCopy copy_region = {0};
  copy_region.size = size;
  vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

  graphics_utils_end_single_time_commands(vulkan_renderer->device, vulkan_renderer->graphics_queue, vulkan_renderer->command_pool, command_buffer);
}

void vulkan_sync_objects_cleanup(struct VulkanRenderer* vulkan_renderer) {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(vulkan_renderer->device, vulkan_renderer->render_finished_semaphores[i], NULL);
    vkDestroySemaphore(vulkan_renderer->device, vulkan_renderer->image_available_semaphores[i], NULL);
    vkDestroyFence(vulkan_renderer->device, vulkan_renderer->in_flight_fences[i], NULL);
  }
}

void vulkan_index_buffer_cleanup(struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, vulkan_renderer->index_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, vulkan_renderer->index_buffer_memory, NULL);
}

void vulkan_vertex_buffer_cleanup(struct VulkanRenderer* vulkan_renderer) {
  vkDestroyBuffer(vulkan_renderer->device, vulkan_renderer->vertex_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, vulkan_renderer->vertex_buffer_memory, NULL);
}

void vulkan_texture_cleanup(struct VulkanRenderer* vulkan_renderer) {
  texture_delete(vulkan_renderer, vulkan_renderer->image_texture);
  free(vulkan_renderer->image_texture);
}
void vulkan_command_pool_cleanup(struct VulkanRenderer* vulkan_renderer) {
  vkDestroyCommandPool(vulkan_renderer->device, vulkan_renderer->command_pool, NULL);
}

void vulkan_descriptor_set_layout_cleanup(struct VulkanRenderer* vulkan_renderer) {
  vkDestroyDescriptorSetLayout(vulkan_renderer->device, vulkan_renderer->descriptor_set_layout, NULL);
  vkDestroyDescriptorPool(vulkan_renderer->device, vulkan_renderer->descriptor_pool, NULL);
}

void vulkan_swap_chain_cleanup(struct VulkanRenderer* vulkan_renderer) {
  // Test: Wait for frame to finish rendering before cleaning up
  for (int loop_num = 0; loop_num < MAX_FRAMES_IN_FLIGHT; loop_num++)
    vkWaitForFences(vulkan_renderer->device, 1, &vulkan_renderer->in_flight_fences[loop_num], VK_TRUE, UINT64_MAX);

  vkDestroyImageView(vulkan_renderer->device, vulkan_renderer->depth_image_view, NULL);
  vkDestroyImage(vulkan_renderer->device, vulkan_renderer->depth_image, NULL);
  vkFreeMemory(vulkan_renderer->device, vulkan_renderer->depth_image_memory, NULL);

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    vkDestroyFramebuffer(vulkan_renderer->device, vulkan_renderer->swap_chain_framebuffers[loopNum], NULL);

  vkFreeCommandBuffers(vulkan_renderer->device, vulkan_renderer->command_pool, 3, vulkan_renderer->command_buffers);

  vkDestroyPipeline(vulkan_renderer->device, vulkan_renderer->graphics_pipeline, NULL);
  vkDestroyPipelineLayout(vulkan_renderer->device, vulkan_renderer->pipeline_layout, NULL);
  vkDestroyRenderPass(vulkan_renderer->device, vulkan_renderer->render_pass, NULL);

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    vkDestroyImageView(vulkan_renderer->device, vulkan_renderer->swap_chain_image_views[loopNum], NULL);

  vkDestroySwapchainKHR(vulkan_renderer->device, vulkan_renderer->swap_chain, NULL);
}

void vulkan_device_cleanup(struct VulkanRenderer* vulkan_renderer) {
  vkDestroyDevice(vulkan_renderer->device, NULL);
}

void vulkan_surface_cleanup(struct VulkanRenderer* vulkan_renderer) {
  vkDestroySurfaceKHR(vulkan_renderer->instance, vulkan_renderer->surface, NULL);
}

void vulkan_debug_cleanup(struct VulkanRenderer* vulkan_renderer) {
  if (enable_validation_layers)
    destroy_debug_utils_messenger_ext(vulkan_renderer->instance, vulkan_renderer->debug_messenger, NULL);
}

void window_cleanup(struct VulkanRenderer* vulkan_renderer) {
  mesh_delete(vulkan_renderer->image_mesh);
  free(vulkan_renderer->image_mesh);

  glfwDestroyWindow(vulkan_renderer->glfw_window);
  glfwTerminate();
}

int create_instance(struct VulkanRenderer* vulkan_renderer) {
  struct VkApplicationInfo appInfo = {0};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Grindstone";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Mana";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  struct VkInstanceCreateInfo create_info = {0};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &appInfo;

  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  const char* convertedExtensions[3];
  memset(convertedExtensions, 0, sizeof(convertedExtensions));

  for (int loopNum = 0; loopNum < glfw_extension_count; loopNum++)
    convertedExtensions[loopNum] = glfw_extensions[loopNum];

  if (enable_validation_layers) {
    convertedExtensions[glfw_extension_count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;  //"VK_EXT_DEBUG_UTILS_EXTENSION_NAME\0";
    glfw_extension_count++;
  }

  create_info.enabledExtensionCount = glfw_extension_count;
  create_info.ppEnabledExtensionNames = convertedExtensions;

  if (enable_validation_layers) {
    create_info.enabledLayerCount = (uint32_t)1;
    create_info.ppEnabledLayerNames = validation_layers;
  } else
    create_info.enabledLayerCount = 0;

  if (vkCreateInstance(&create_info, NULL, &vulkan_renderer->instance) != VK_SUCCESS)
    return CREATE_INSTANCE_ERROR;

  return NO_ERROR;
}
int setup_debug_messenger(struct VulkanRenderer* vulkan_renderer) {
  if (enable_validation_layers && !check_validation_layer_support())
    return SETUP_DEBUG_MESSENGER_ERROR;

  if (enable_validation_layers) {
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {0};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = debug_callback;

    if (create_debug_utils_messenger_ext(vulkan_renderer->instance, &debugInfo, NULL, &vulkan_renderer->debug_messenger) != VK_SUCCESS)
      return SETUP_DEBUG_MESSENGER_ERROR;
  }

  return NO_ERROR;
}
int create_surface(struct VulkanRenderer* vulkan_renderer) {
  if (glfwCreateWindowSurface(vulkan_renderer->instance, vulkan_renderer->glfw_window, NULL, &vulkan_renderer->surface) != VK_SUCCESS)
    return CREATE_SURFACE_ERROR;

  return NO_ERROR;
}
int pick_physical_device(struct VulkanRenderer* vulkan_renderer) {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(vulkan_renderer->instance, &device_count, NULL);

  if (device_count == 0)
    return PICK_PHYSICAL_DEVICE_ERROR;

  VkPhysicalDevice devices[device_count];
  memset(devices, 0, sizeof(devices));

  vkEnumeratePhysicalDevices(vulkan_renderer->instance, &device_count, devices);

  for (int loopNum = 0; loopNum < device_count; loopNum++) {
    if (is_device_suitable(vulkan_renderer, devices[loopNum])) {
      vulkan_renderer->physical_device = devices[loopNum];
      break;
    }
  }

  if (vulkan_renderer->physical_device == VK_NULL_HANDLE)
    return PICK_PHYSICAL_DEVICE_ERROR;

  return NO_ERROR;
}
int create_logical_device(struct VulkanRenderer* vulkan_renderer) {
  // Create device
  //TODO: LOOK MORE INTO THIS NOT CORRECT
  int queue_create_infos_size = 1;
  VkDeviceQueueCreateInfo queueCreateInfos[queue_create_infos_size];
  memset(queueCreateInfos, 0, sizeof(queueCreateInfos));

  float queuePriority = 1.0f;

  queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfos[0].queueFamilyIndex = (&vulkan_renderer->indices)->graphics_family;
  queueCreateInfos[0].queueCount = 1;
  queueCreateInfos[0].pQueuePriorities = &queuePriority;

  //if ((&vulkan_renderer->indices)->graphicsFamily != (&vulkan_renderer->indices)->presentFamily) {
  //    queueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  //    queueCreateInfos[1].queueFamilyIndex = (&vulkan_renderer->indices)->presentFamily;
  //    queueCreateInfos[1].queueCount = 1;
  //    queueCreateInfos[1].pQueuePriorities = &queuePriority;
  //}

  struct VkPhysicalDeviceFeatures device_features = {0};
  device_features.samplerAnisotropy = VK_TRUE;

  struct VkDeviceCreateInfo deviceInfo = {0};
  deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  //if ((&vulkan_renderer->indices)->graphicsFamily != (&vulkan_renderer->indices)->presentFamily)
  //    deviceInfo.queueCreateInfoCount = (uint32_t)(queueCreateInfosSize);
  //else
  deviceInfo.queueCreateInfoCount = 1;
  deviceInfo.pQueueCreateInfos = queueCreateInfos;

  deviceInfo.pEnabledFeatures = &device_features;

  deviceInfo.enabledExtensionCount = (int32_t)1;
  deviceInfo.ppEnabledExtensionNames = device_extensions;

  if (enable_validation_layers) {
    deviceInfo.enabledLayerCount = (uint32_t)1;
    deviceInfo.ppEnabledLayerNames = validation_layers;
  } else
    deviceInfo.enabledLayerCount = 0;

  if (vkCreateDevice(vulkan_renderer->physical_device, &deviceInfo, NULL, &vulkan_renderer->device) != VK_SUCCESS)
    return CREATE_LOGICAL_DEVICE_ERROR;

  vkGetDeviceQueue(vulkan_renderer->device, vulkan_renderer->indices.graphics_family, 0, &vulkan_renderer->graphics_queue);
  vkGetDeviceQueue(vulkan_renderer->device, vulkan_renderer->indices.present_family, 0, &vulkan_renderer->present_queue);

  return NO_ERROR;
}

int create_swap_chain(struct VulkanRenderer* vulkan_renderer, int width, int height) {
  struct SwapChainSupportDetails swap_chain_support = {{0}};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &swap_chain_support.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &formatCount, NULL);

  if (formatCount != 0)
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &formatCount, swap_chain_support.formats);

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &presentModeCount, NULL);

  if (presentModeCount != 0)
    vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &presentModeCount, swap_chain_support.present_modes);

  VkSurfaceFormatKHR surfaceFormat = {0};

  if (formatCount == 1 && swap_chain_support.formats[0].format == VK_FORMAT_UNDEFINED) {
    surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  } else {
    for (int loopNum = 0; loopNum < SMALL_BUFFER; loopNum++) {
      if (swap_chain_support.formats[loopNum].format == VK_FORMAT_B8G8R8A8_UNORM && swap_chain_support.formats[loopNum].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        surfaceFormat = swap_chain_support.formats[loopNum];
        break;
      } else
        surfaceFormat = swap_chain_support.formats[0];
    }
  }

  VkPresentModeKHR presentMode = {0};

  for (int loopNum = 0; loopNum < SMALL_BUFFER; loopNum++) {
    if (swap_chain_support.present_modes[loopNum] == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = swap_chain_support.present_modes[loopNum];
      break;
    } else if (swap_chain_support.present_modes[loopNum] == VK_PRESENT_MODE_IMMEDIATE_KHR)
      presentMode = swap_chain_support.present_modes[loopNum];
  }

  // Force Vsync
  presentMode = VK_PRESENT_MODE_FIFO_KHR;

  VkExtent2D extent = {width, height};
  if (swap_chain_support.capabilities.currentExtent.width != UINT32_MAX)
    extent = swap_chain_support.capabilities.currentExtent;
  else {
    extent.width = MAX(swap_chain_support.capabilities.minImageExtent.width, MIN(swap_chain_support.capabilities.maxImageExtent.width, extent.width));
    extent.height = MAX(swap_chain_support.capabilities.minImageExtent.height, MIN(swap_chain_support.capabilities.maxImageExtent.height, extent.height));
  }

  uint32_t imageCount = swap_chain_support.capabilities.minImageCount + 1;
  if (swap_chain_support.capabilities.maxImageCount > 0 && imageCount > swap_chain_support.capabilities.maxImageCount)
    imageCount = swap_chain_support.capabilities.maxImageCount;

  VkSwapchainCreateInfoKHR swapchainInfo = {0};
  swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainInfo.surface = vulkan_renderer->surface;

  swapchainInfo.minImageCount = imageCount;
  swapchainInfo.imageFormat = surfaceFormat.format;
  swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapchainInfo.imageExtent = extent;
  swapchainInfo.imageArrayLayers = 1;
  swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamilyIndices[] = {(&vulkan_renderer->indices)->graphics_family, (&vulkan_renderer->indices)->present_family};

  if ((&vulkan_renderer->indices)->graphics_family != (&vulkan_renderer->indices)->present_family) {
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainInfo.queueFamilyIndexCount = 2;
    swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

  swapchainInfo.preTransform = swap_chain_support.capabilities.currentTransform;
  swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainInfo.presentMode = presentMode;
  swapchainInfo.clipped = VK_TRUE;

  if (vkCreateSwapchainKHR(vulkan_renderer->device, &swapchainInfo, NULL, &vulkan_renderer->swap_chain) != VK_SUCCESS)
    return CREATE_SWAP_CHAIN_ERROR;

  vkGetSwapchainImagesKHR(vulkan_renderer->device, vulkan_renderer->swap_chain, &imageCount, NULL);
  vkGetSwapchainImagesKHR(vulkan_renderer->device, vulkan_renderer->swap_chain, &imageCount, vulkan_renderer->swap_chain_images);

  vulkan_renderer->swap_chain_image_format = surfaceFormat.format;
  vulkan_renderer->swap_chain_extent = extent;

  return NO_ERROR;
}

int create_image_views(struct VulkanRenderer* vulkan_renderer) {
  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = vulkan_renderer->swap_chain_images[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = vulkan_renderer->swap_chain_image_format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkan_renderer->device, &viewInfo, NULL, &vulkan_renderer->swap_chain_image_views[i]) != VK_SUCCESS)
      return CREATE_IMAGE_VIEWS_ERROR;
  }

  return NO_ERROR;
}

int create_render_pass(struct VulkanRenderer* vulkan_renderer) {
  VkAttachmentDescription color_attachment = {0};
  color_attachment.format = vulkan_renderer->swap_chain_image_format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depth_attachment = {0};
  depth_attachment.format = find_depth_format(vulkan_renderer);
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference color_attachment_ref = {0};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depth_attachment_ref = {0};
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {0};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;
  subpass.pDepthStencilAttachment = &depth_attachment_ref;

  VkSubpassDependency dependency = {0};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachments[2] = {color_attachment, depth_attachment};
  VkRenderPassCreateInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = 2;
  render_pass_info.pAttachments = attachments;
  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;
  render_pass_info.dependencyCount = 1;
  render_pass_info.pDependencies = &dependency;

  if (vkCreateRenderPass(vulkan_renderer->device, &render_pass_info, NULL, &vulkan_renderer->render_pass) != VK_SUCCESS)
    return CREATE_RENDER_PASS_ERROR;

  return NO_ERROR;
}

int create_descriptor_set_layout(struct VulkanRenderer* vulkan_renderer) {
  VkDescriptorSetLayoutBinding ubo_layout_binding = {0};
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.pImmutableSamplers = NULL;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding sampler_layout_binding = {0};
  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.pImmutableSamplers = NULL;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding bindings[2] = {ubo_layout_binding, sampler_layout_binding};
  VkDescriptorSetLayoutCreateInfo layout_info = {0};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = 2;
  layout_info.pBindings = bindings;

  if (vkCreateDescriptorSetLayout(vulkan_renderer->device, &layout_info, NULL, &vulkan_renderer->descriptor_set_layout) != VK_SUCCESS)
    return -1;

  return 0;
}

VkShaderModule create_shader_module(struct VulkanRenderer* vulkan_renderer, const char* code, int length) {
  VkShaderModuleCreateInfo create_info = {0};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = length;
  create_info.pCode = (const uint32_t*)(code);

  VkShaderModule shader_module;
  if (vkCreateShaderModule(vulkan_renderer->device, &create_info, NULL, &shader_module) != VK_SUCCESS)
    fprintf(stderr, "Failed to create shader module!\n");

  return shader_module;
}

char* read_shader_file(const char* filename, int* file_length) {
  FILE* fp = fopen(filename, "rb");

  fseek(fp, 0, SEEK_END);
  long int size = ftell(fp);
  rewind(fp);

  *file_length = size;

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

int create_graphics_pipeline(struct VulkanRenderer* vulkan_renderer) {
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

  int vertex_length = 0;
  int fragment_length = 0;

  char* vert_shader_code = read_shader_file("./assets/shaders/spirv/texture.vert.spv", &vertex_length);
  char* frag_shader_code = read_shader_file("./assets/shaders/spirv/texture.frag.spv", &fragment_length);

  VkShaderModule vert_shader_module = create_shader_module(vulkan_renderer, vert_shader_code, vertex_length);
  VkShaderModule frag_shader_module = create_shader_module(vulkan_renderer, frag_shader_code, fragment_length);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vert_shader_module;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, frag_shader_stage_info};

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkVertexInputBindingDescription binding_description = get_binding_description();
  VkVertexInputAttributeDescription attribute_descriptions[5];
  memset(attribute_descriptions, 0, sizeof(attribute_descriptions));
  get_attribute_descriptions(attribute_descriptions);

  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.vertexAttributeDescriptionCount = 5;  // Note: length of attributeDescriptions
  vertex_input_info.pVertexBindingDescriptions = &binding_description;
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions;

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {0};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)vulkan_renderer->swap_chain_extent.width;
  viewport.height = (float)vulkan_renderer->swap_chain_extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent = vulkan_renderer->swap_chain_extent;

  VkPipelineViewportStateCreateInfo viewport_state = {0};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {0};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {0};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
  depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil.depthTestEnable = VK_TRUE;
  depth_stencil.depthWriteEnable = VK_TRUE;
  depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil.depthBoundsTestEnable = VK_FALSE;
  depth_stencil.stencilTestEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending = {0};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.logicOp = VK_LOGIC_OP_COPY;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &vulkan_renderer->descriptor_set_layout;

  if (vkCreatePipelineLayout(vulkan_renderer->device, &pipeline_layout_info, NULL, &vulkan_renderer->pipeline_layout) != VK_SUCCESS)
    return CREATE_GRAPHICS_PIPELINE_ERROR;

  VkGraphicsPipelineCreateInfo pipelineInfo = {0};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertex_input_info;
  pipelineInfo.pInputAssemblyState = &input_assembly;
  pipelineInfo.pViewportState = &viewport_state;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depth_stencil;
  pipelineInfo.pColorBlendState = &color_blending;
  pipelineInfo.layout = vulkan_renderer->pipeline_layout;
  pipelineInfo.renderPass = vulkan_renderer->render_pass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(vulkan_renderer->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &vulkan_renderer->graphics_pipeline) != VK_SUCCESS)
    return CREATE_GRAPHICS_PIPELINE_ERROR;

  vkDestroyShaderModule(vulkan_renderer->device, frag_shader_module, NULL);
  vkDestroyShaderModule(vulkan_renderer->device, vert_shader_module, NULL);

  return NO_ERROR;
}

int create_framebuffers(struct VulkanRenderer* vulkan_renderer) {
  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++) {
    VkImageView attachments[] = {vulkan_renderer->swap_chain_image_views[loopNum], vulkan_renderer->depth_image_view};

    VkFramebufferCreateInfo framebufferInfo = {0};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vulkan_renderer->render_pass;
    framebufferInfo.attachmentCount = 2;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = vulkan_renderer->swap_chain_extent.width;
    framebufferInfo.height = vulkan_renderer->swap_chain_extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vulkan_renderer->device, &framebufferInfo, NULL, &vulkan_renderer->swap_chain_framebuffers[loopNum]) != VK_SUCCESS)
      return CREATE_FRAME_BUFFER_ERROR;
  }

  return NO_ERROR;
}

int create_command_pool(struct VulkanRenderer* vulkan_renderer) {
  VkCommandPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = vulkan_renderer->indices.graphics_family;

  if (vkCreateCommandPool(vulkan_renderer->device, &poolInfo, NULL, &vulkan_renderer->command_pool) != VK_SUCCESS)
    return CREATE_COMMAND_POOL_ERROR;

  return NO_ERROR;
}

int create_depth_resources(struct VulkanRenderer* vulkan_renderer) {
  VkFormat depthFormat = find_depth_format(vulkan_renderer);

  graphics_utils_create_image(vulkan_renderer->device, vulkan_renderer->physical_device, vulkan_renderer->swap_chain_extent.width, vulkan_renderer->swap_chain_extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vulkan_renderer->depth_image, &vulkan_renderer->depth_image_memory);
  vulkan_renderer->depth_image_view = graphics_utils_create_image_view(vulkan_renderer->device, vulkan_renderer->depth_image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

  graphics_utils_transition_image_layout(vulkan_renderer->device, vulkan_renderer->graphics_queue, vulkan_renderer->command_pool, &vulkan_renderer->depth_image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

  return 0;
}

int create_command_buffers(struct VulkanRenderer* vulkan_renderer) {
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = vulkan_renderer->command_pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)MAX_SWAP_CHAIN_FRAMES;

  if (vkAllocateCommandBuffers(vulkan_renderer->device, &allocInfo, vulkan_renderer->command_buffers) != VK_SUCCESS)
    return CREATE_COMMAND_BUFFER_ERROR;

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(vulkan_renderer->command_buffers[i], &beginInfo) != VK_SUCCESS)
      return CREATE_COMMAND_BUFFER_ERROR;

    VkRenderPassBeginInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkan_renderer->render_pass;
    renderPassInfo.framebuffer = vulkan_renderer->swap_chain_framebuffers[i];
    renderPassInfo.renderArea.offset.x = 0;
    renderPassInfo.renderArea.offset.y = 0;
    renderPassInfo.renderArea.extent = vulkan_renderer->swap_chain_extent;

    VkClearValue clear_values[2];
    memset(clear_values, 0, sizeof(clear_values));

    //http://ogldev.atspace.co.uk/www/tutorial51/tutorial51.html
    VkClearColorValue clear_color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clear_values[0].color = clear_color;

    VkClearDepthStencilValue depth_color = {1.0f, 0};
    clear_values[1].depthStencil = depth_color;

    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clear_values;

    vkCmdBeginRenderPass(vulkan_renderer->command_buffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(vulkan_renderer->command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_renderer->graphics_pipeline);

    VkBuffer vertexBuffers[] = {vulkan_renderer->vertex_buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(vulkan_renderer->command_buffers[i], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(vulkan_renderer->command_buffers[i], vulkan_renderer->index_buffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(vulkan_renderer->command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_renderer->pipeline_layout, 0, 1, &vulkan_renderer->descriptor_sets[i], 0, NULL);

    vkCmdDrawIndexed(vulkan_renderer->command_buffers[i], vulkan_renderer->image_mesh->indices->size, 1, 0, 0, 0);
    //vkCmdDrawIndexed(vulkan_renderer->commandBuffers[i], 12, 1, 0, 0, 0);
    vkCmdEndRenderPass(vulkan_renderer->command_buffers[i]);

    if (vkEndCommandBuffer(vulkan_renderer->command_buffers[i]) != VK_SUCCESS)
      return CREATE_COMMAND_BUFFER_ERROR;
  }

  return NO_ERROR;
}

int create_sync_objects(struct VulkanRenderer* vulkan_renderer) {
  memset(vulkan_renderer->in_flight_fences, 0, sizeof(vulkan_renderer->in_flight_fences));

  VkSemaphoreCreateInfo semaphore_info = {0};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info = {0};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(vulkan_renderer->device, &semaphore_info, NULL, &vulkan_renderer->image_available_semaphores[i]) != VK_SUCCESS || vkCreateSemaphore(vulkan_renderer->device, &semaphore_info, NULL, &vulkan_renderer->render_finished_semaphores[i]) != VK_SUCCESS || vkCreateFence(vulkan_renderer->device, &fence_info, NULL, &vulkan_renderer->in_flight_fences[i]) != VK_SUCCESS)
      return CREATE_SYNC_OBJECT_ERROR;
  }

  return NO_ERROR;
}

bool is_device_suitable(struct VulkanRenderer* vulkan_renderer, VkPhysicalDevice device) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

  struct VkQueueFamilyProperties queue_families[queue_family_count];
  memset(queue_families, 0, sizeof(queue_families));

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

  bool graphics_family_found = false;
  bool present_family_found = false;

  int i = 0;
  for (int loopNum = 0; loopNum < queue_family_count; loopNum++) {
    if (queue_families[loopNum].queueCount > 0 && queue_families[loopNum].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      (&vulkan_renderer->indices)->graphics_family = i;
      graphics_family_found = true;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vulkan_renderer->surface, &present_support);

    if (queue_families[loopNum].queueCount > 0 && present_support) {
      (&vulkan_renderer->indices)->present_family = i;
      present_family_found = true;
    }

    if (graphics_family_found && present_family_found)
      break;

    i++;
  }

  if (!graphics_family_found || !present_family_found)
    return false;

  return true;
}

bool check_validation_layer_support() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);

  VkLayerProperties available_layers[layer_count];
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

  bool layerFound = false;

  for (int loop_num = 0; loop_num < layer_count; loop_num++) {
    if (strcmp(validation_layers[0], available_layers[loop_num].layerName) == 0) {
      layerFound = true;
      break;
    }
  }

  return layerFound;
}

int create_vertex_buffer(struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = vulkan_renderer->image_mesh->vertices->memory_size * vulkan_renderer->image_mesh->vertices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageVertices.items[0]) * vulkan_renderer->imageVertices.total;

  VkBuffer staging_buffer = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &stagingBufferMemory);

  void* data;
  vkMapMemory(vulkan_renderer->device, stagingBufferMemory, 0, buffer_size, 0, &data);
  memcpy(data, vulkan_renderer->image_mesh->vertices->items, buffer_size);
  vkUnmapMemory(vulkan_renderer->device, stagingBufferMemory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vulkan_renderer->vertex_buffer, &vulkan_renderer->vertex_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer, vulkan_renderer->vertex_buffer, buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, stagingBufferMemory, NULL);

  return 0;
}

int create_index_buffer(struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = vulkan_renderer->image_mesh->indices->memory_size * vulkan_renderer->image_mesh->indices->size;
  //VkDeviceSize bufferSize = sizeof(vulkan_renderer->imageIndices.items[0]) * vulkan_renderer->imageIndices.total;

  VkBuffer staging_buffer = {0};
  VkDeviceMemory staging_buffer_memory = {0};
  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_buffer_memory);

  void* data;
  vkMapMemory(vulkan_renderer->device, staging_buffer_memory, 0, buffer_size, 0, &data);
  memcpy(data, vulkan_renderer->image_mesh->indices->items, buffer_size);
  vkUnmapMemory(vulkan_renderer->device, staging_buffer_memory);

  graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vulkan_renderer->index_buffer, &vulkan_renderer->index_buffer_memory);

  copy_buffer(vulkan_renderer, staging_buffer, vulkan_renderer->index_buffer, buffer_size);

  vkDestroyBuffer(vulkan_renderer->device, staging_buffer, NULL);
  vkFreeMemory(vulkan_renderer->device, staging_buffer_memory, NULL);

  return 0;
}

int create_uniform_buffers(struct VulkanRenderer* vulkan_renderer) {
  VkDeviceSize buffer_size = sizeof(struct UniformBufferObject);

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++)
    graphics_utils_create_buffer(vulkan_renderer->device, vulkan_renderer->physical_device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vulkan_renderer->uniform_buffers[i], &vulkan_renderer->uniform_buffers_memory[i]);

  return 0;
}

int create_descriptor_pool(struct VulkanRenderer* vulkan_renderer) {
  VkDescriptorPoolSize pool_sizes[2];
  memset(pool_sizes, 0, sizeof(pool_sizes));

  pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  pool_sizes[0].descriptorCount = MAX_SWAP_CHAIN_FRAMES;
  pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  pool_sizes[1].descriptorCount = MAX_SWAP_CHAIN_FRAMES;

  VkDescriptorPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 2;
  poolInfo.pPoolSizes = pool_sizes;
  poolInfo.maxSets = MAX_SWAP_CHAIN_FRAMES;

  if (vkCreateDescriptorPool(vulkan_renderer->device, &poolInfo, NULL, &vulkan_renderer->descriptor_pool) != VK_SUCCESS) {
    fprintf(stderr, "failed to create descriptor pool!\n");
    return -1;
  }

  return 0;
}

int create_descriptor_sets(struct VulkanRenderer* vulkan_renderer) {
  VkDescriptorSetLayout layouts[MAX_SWAP_CHAIN_FRAMES];
  memset(layouts, 0, sizeof(layouts));

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    layouts[loopNum] = vulkan_renderer->descriptor_set_layout;

  VkDescriptorSetAllocateInfo alloc_info = {0};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = vulkan_renderer->descriptor_pool;
  alloc_info.descriptorSetCount = MAX_SWAP_CHAIN_FRAMES;
  alloc_info.pSetLayouts = layouts;

  if (vkAllocateDescriptorSets(vulkan_renderer->device, &alloc_info, vulkan_renderer->descriptor_sets) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate descriptor sets!\n");
    return -1;
  }

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkDescriptorBufferInfo buffer_info = {0};
    buffer_info.buffer = vulkan_renderer->uniform_buffers[i];
    buffer_info.offset = 0;
    buffer_info.range = sizeof(struct UniformBufferObject);

    VkDescriptorImageInfo image_info = {0};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = vulkan_renderer->image_texture->textureImageView;
    image_info.sampler = vulkan_renderer->image_texture->textureSampler;

    //int descriptorSize = 2;
    struct Vector descriptor_writes;
    vector_init(&descriptor_writes, sizeof(VkWriteDescriptorSet));
    //VkWriteDescriptorSet descriptorWrites[descriptorSize];
    //memset(descriptorWrites, 0, sizeof(descriptorWrites));

    VkWriteDescriptorSet dcs1 = {0};
    dcs1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs1.dstSet = vulkan_renderer->descriptor_sets[i];
    dcs1.dstBinding = descriptor_writes.size;
    dcs1.dstArrayElement = 0;
    dcs1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dcs1.descriptorCount = 1;
    dcs1.pBufferInfo = &buffer_info;

    vector_push_back(&descriptor_writes, &dcs1);

    VkWriteDescriptorSet dcs2 = {0};
    dcs2.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    dcs2.dstSet = vulkan_renderer->descriptor_sets[i];
    dcs2.dstBinding = descriptor_writes.size;
    dcs2.dstArrayElement = 0;
    dcs2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dcs2.descriptorCount = 1;
    dcs2.pImageInfo = &image_info;

    vector_push_back(&descriptor_writes, &dcs2);

    vkUpdateDescriptorSets(vulkan_renderer->device, descriptor_writes.size, descriptor_writes.items, 0, NULL);
  }

  return 0;
}

void recreate_swap_chain(struct VulkanRenderer* vulkan_renderer) {
  int width = 0, height = 0;
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(vulkan_renderer->glfw_window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(vulkan_renderer->device);

  vulkan_swap_chain_cleanup(vulkan_renderer);

  create_swap_chain(vulkan_renderer, width, height);
  create_image_views(vulkan_renderer);
  create_render_pass(vulkan_renderer);
  create_graphics_pipeline(vulkan_renderer);
  create_depth_resources(vulkan_renderer);
  create_framebuffers(vulkan_renderer);
  create_command_buffers(vulkan_renderer);
}

VkVertexInputBindingDescription get_binding_description() {
  VkVertexInputBindingDescription binding_description = {0};
  binding_description.binding = 0;
  binding_description.stride = sizeof(struct Vertex);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return binding_description;
}

void get_attribute_descriptions(VkVertexInputAttributeDescription* attribute_descriptions) {
  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[0].offset = offsetof(struct Vertex, position);

  attribute_descriptions[1].binding = 0;
  attribute_descriptions[1].location = 1;
  attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[1].offset = offsetof(struct Vertex, normal);

  attribute_descriptions[2].binding = 0;
  attribute_descriptions[2].location = 2;
  attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[2].offset = offsetof(struct Vertex, tex_coord);

  attribute_descriptions[3].binding = 0;
  attribute_descriptions[3].location = 3;
  attribute_descriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[3].offset = offsetof(struct Vertex, tangent);

  attribute_descriptions[4].binding = 0;
  attribute_descriptions[4].location = 4;
  attribute_descriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[4].offset = offsetof(struct Vertex, bit_tangent);
}

#define TOTAL_CANDIDIATES 3
VkFormat find_depth_format(struct VulkanRenderer* vulkan_renderer) {
  VkFormat candidate[TOTAL_CANDIDIATES] = {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
  VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
  VkFormatFeatureFlags features =
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

  for (int loopNum = 0; loopNum < TOTAL_CANDIDIATES; loopNum++) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(vulkan_renderer->physical_device, candidate[loopNum], &props);

    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (props.linearTilingFeatures & features) == features) {
      return candidate[loopNum];
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return candidate[loopNum];
    }
  }

  // printf("failed to find supported format!");

  // return 0;
  return -1;
}
