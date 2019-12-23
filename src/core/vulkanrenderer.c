#include "core/vulkanrenderer.h"

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  struct VulkanRenderer* vulkan_renderer_handle = (struct VulkanRenderer*)(glfwGetWindowUserPointer(window));
  vulkan_renderer_handle->framebuffer_resized = true;
}

int vulkan_renderer_init(struct VulkanRenderer* vulkan_renderer, int width, int height) {
  memset(vulkan_renderer, 0, sizeof(struct VulkanRenderer));
  vulkan_renderer->swapchain = calloc(sizeof(struct SwapChain), 1);

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
    return VULKAN_RENDERER_CREATE_WINDOW_ERROR;

  int error_code;

  if ((error_code = create_instance(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto window_error;
  if ((error_code = setup_debug_messenger(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_debug_error;
  if ((error_code = create_surface(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_surface_error;
  if ((error_code = pick_physical_device(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_surface_error;
  if ((error_code = create_logical_device(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_device_error;
  if ((error_code = create_swap_chain(vulkan_renderer, width, height)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_swap_chain_error;
  swapchain_init(vulkan_renderer->swapchain, vulkan_renderer);
  ////////////////////////////
  if ((error_code = create_command_pool(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_command_pool_error;
  ////////////////////////////
  //create_depth_resources(vulkan_renderer);
  if ((error_code = create_framebuffers(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_command_pool_error;
  if ((error_code = create_command_buffers(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_command_pool_error;
  if ((error_code = create_sync_objects(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_sync_objects_error;

  return VULKAN_RENDERER_SUCCESS;

vulkan_sync_objects_error:
  vulkan_sync_objects_cleanup(vulkan_renderer);
vulkan_command_pool_error:
  vulkan_command_pool_cleanup(vulkan_renderer);
vulkan_swap_chain_error:
  vulkan_swap_chain_cleanup(vulkan_renderer);
vulkan_device_error:
  vulkan_device_cleanup(vulkan_renderer);
vulkan_surface_error:
  vulkan_surface_cleanup(vulkan_renderer);
vulkan_debug_error:
  vulkan_debug_cleanup(vulkan_renderer);
window_error:
  window_cleanup(vulkan_renderer);

  return error_code;
}

// TODO: Clean up da poopoo code
void vulkan_renderer_delete(struct VulkanRenderer* vulkan_renderer) {
  vulkan_swap_chain_cleanup(vulkan_renderer);
  //vulkan_descriptor_set_layout_cleanup(vulkan_renderer);
  vulkan_sync_objects_cleanup(vulkan_renderer);
  vulkan_command_pool_cleanup(vulkan_renderer);
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

void vulkan_swap_chain_cleanup(struct VulkanRenderer* vulkan_renderer) {
  // Wait for frame to finish rendering before cleaning up
  for (int loop_num = 0; loop_num < MAX_FRAMES_IN_FLIGHT; loop_num++)
    vkWaitForFences(vulkan_renderer->device, 1, &vulkan_renderer->in_flight_fences[loop_num], VK_TRUE, UINT64_MAX);

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    vkDestroyFramebuffer(vulkan_renderer->device, vulkan_renderer->swapchain->swap_chain_framebuffers[loopNum], NULL);

  vkFreeCommandBuffers(vulkan_renderer->device, vulkan_renderer->command_pool, 3, vulkan_renderer->command_buffers);

  swapchain_delete(vulkan_renderer->swapchain, vulkan_renderer);

  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++)
    vkDestroyImageView(vulkan_renderer->device, vulkan_renderer->swapchain->swap_chain_image_views[loopNum], NULL);

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
void vulkan_command_pool_cleanup(struct VulkanRenderer* vulkan_renderer) {
  vkDestroyCommandPool(vulkan_renderer->device, vulkan_renderer->command_pool, NULL);
}

void window_cleanup(struct VulkanRenderer* vulkan_renderer) {
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
    return VULKAN_RENDERER_CREATE_INSTANCE_ERROR;

  return VULKAN_RENDERER_SUCCESS;
}
int setup_debug_messenger(struct VulkanRenderer* vulkan_renderer) {
  if (enable_validation_layers && !check_validation_layer_support())
    return VULKAN_RENDERER_SETUP_DEBUG_MESSENGER_ERROR;

  if (enable_validation_layers) {
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {0};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = debug_callback;

    if (create_debug_utils_messenger_ext(vulkan_renderer->instance, &debugInfo, NULL, &vulkan_renderer->debug_messenger) != VK_SUCCESS)
      return VULKAN_RENDERER_SETUP_DEBUG_MESSENGER_ERROR;
  }

  return VULKAN_RENDERER_SUCCESS;
}
int create_surface(struct VulkanRenderer* vulkan_renderer) {
  if (glfwCreateWindowSurface(vulkan_renderer->instance, vulkan_renderer->glfw_window, NULL, &vulkan_renderer->surface) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_SURFACE_ERROR;

  return VULKAN_RENDERER_SUCCESS;
}
int pick_physical_device(struct VulkanRenderer* vulkan_renderer) {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(vulkan_renderer->instance, &device_count, NULL);

  if (device_count == 0)
    return VULKAN_RENDERER_PICK_PHYSICAL_DEVICE_ERROR;

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
    return VULKAN_RENDERER_PICK_PHYSICAL_DEVICE_ERROR;

  return VULKAN_RENDERER_SUCCESS;
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
    return VULKAN_RENDERER_CREATE_LOGICAL_DEVICE_ERROR;

  vkGetDeviceQueue(vulkan_renderer->device, vulkan_renderer->indices.graphics_family, 0, &vulkan_renderer->graphics_queue);
  vkGetDeviceQueue(vulkan_renderer->device, vulkan_renderer->indices.present_family, 0, &vulkan_renderer->present_queue);

  return VULKAN_RENDERER_SUCCESS;
}

int create_swap_chain(struct VulkanRenderer* vulkan_renderer, int width, int height) {
  struct SwapChainSupportDetails swap_chain_support = {{0}};

  vector_init(&swap_chain_support.formats, sizeof(struct VkSurfaceFormatKHR));
  vector_init(&swap_chain_support.present_modes, sizeof(enum VkPresentModeKHR));

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &swap_chain_support.capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &format_count, NULL);

  if (format_count != 0) {
    vector_resize(&swap_chain_support.formats, format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &format_count, swap_chain_support.formats.items);
    swap_chain_support.formats.size = format_count;
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &present_mode_count, NULL);

  if (present_mode_count != 0) {
    vector_resize(&swap_chain_support.present_modes, present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_renderer->physical_device, vulkan_renderer->surface, &present_mode_count, swap_chain_support.present_modes.items);
    swap_chain_support.present_modes.size = present_mode_count;
  }

  VkSurfaceFormatKHR surface_format = {0};

  //if (format_count == 1 && swap_chain_support.formats[0].format == VK_FORMAT_UNDEFINED) {
  if (format_count == 1 && ((struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, 0))->format == VK_FORMAT_UNDEFINED) {
    surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
    surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  } else {
    for (int loop_num = 0; loop_num < vector_size(&swap_chain_support.formats); loop_num++) {
      if (((struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, loop_num))->format == VK_FORMAT_B8G8R8A8_UNORM && ((struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, loop_num))->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        surface_format = *(struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, loop_num);
        break;
      } else
        surface_format = *(struct VkSurfaceFormatKHR*)vector_get(&swap_chain_support.formats, 0);
    }
  }

  // Testing 10 bit swapchain output
  // Seems to work on Nvidia Studio Driver
  surface_format.format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;

  VkPresentModeKHR present_mode = {0};

  for (int loop_num = 0; loop_num < vector_size(&swap_chain_support.present_modes); loop_num++) {
    if ((enum VkPresentModeKHR)vector_get(&swap_chain_support.present_modes, loop_num) == VK_PRESENT_MODE_MAILBOX_KHR) {
      present_mode = (enum VkPresentModeKHR)vector_get(&swap_chain_support.present_modes, loop_num);
      break;
    } else if ((enum VkPresentModeKHR)vector_get(&swap_chain_support.present_modes, loop_num) == VK_PRESENT_MODE_IMMEDIATE_KHR)
      present_mode = (enum VkPresentModeKHR)vector_get(&swap_chain_support.present_modes, loop_num);
  }

  // Force Vsync
  present_mode = VK_PRESENT_MODE_FIFO_KHR;

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

  VkSwapchainCreateInfoKHR swapchain_info = {0};
  swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_info.surface = vulkan_renderer->surface;

  swapchain_info.minImageCount = imageCount;
  swapchain_info.imageFormat = surface_format.format;
  swapchain_info.imageColorSpace = surface_format.colorSpace;
  swapchain_info.imageExtent = extent;
  swapchain_info.imageArrayLayers = 1;
  swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queue_family_indices[] = {(&vulkan_renderer->indices)->graphics_family, (&vulkan_renderer->indices)->present_family};

  if ((&vulkan_renderer->indices)->graphics_family != (&vulkan_renderer->indices)->present_family) {
    swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchain_info.queueFamilyIndexCount = 2;
    swapchain_info.pQueueFamilyIndices = queue_family_indices;
  } else
    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

  swapchain_info.preTransform = swap_chain_support.capabilities.currentTransform;
  swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_info.presentMode = present_mode;
  swapchain_info.clipped = VK_TRUE;

  if (vkCreateSwapchainKHR(vulkan_renderer->device, &swapchain_info, NULL, &vulkan_renderer->swap_chain) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_SWAP_CHAIN_ERROR;

  vkGetSwapchainImagesKHR(vulkan_renderer->device, vulkan_renderer->swap_chain, &imageCount, NULL);
  vkGetSwapchainImagesKHR(vulkan_renderer->device, vulkan_renderer->swap_chain, &imageCount, vulkan_renderer->swapchain->swap_chain_images);

  vulkan_renderer->swap_chain_image_format = surface_format.format;
  vulkan_renderer->swap_chain_extent = extent;

  vector_delete(&swap_chain_support.formats);
  vector_delete(&swap_chain_support.present_modes);

  return VULKAN_RENDERER_SUCCESS;
}

int create_swapchain_image_views(struct VulkanRenderer* vulkan_renderer, struct VkImage_T* images[MAX_SWAP_CHAIN_FRAMES], struct VkImageView_T* image_view[MAX_SWAP_CHAIN_FRAMES]) {
  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    VkImageViewCreateInfo view_info = {0};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = images[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = vulkan_renderer->swap_chain_image_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkan_renderer->device, &view_info, NULL, image_view) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR;
  }

  return VULKAN_RENDERER_SUCCESS;
}

int create_image_view(struct VulkanRenderer* vulkan_renderer, struct VkImage_T* image, struct VkImageView_T* image_view) {
  VkImageViewCreateInfo view_info = {0};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = vulkan_renderer->swap_chain_image_format;
  view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  if (vkCreateImageView(vulkan_renderer->device, &view_info, NULL, &image_view) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_IMAGE_VIEWS_ERROR;

  return VULKAN_RENDERER_SUCCESS;
}

void create_color_attachment(struct VulkanRenderer* vulkan_renderer, struct VkAttachmentDescription* color_attachment) {
  color_attachment->format = vulkan_renderer->swap_chain_image_format;
  color_attachment->samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

void create_depth_attachment(struct VulkanRenderer* vulkan_renderer, struct VkAttachmentDescription* depth_attachment) {
  depth_attachment->format = find_depth_format(vulkan_renderer);
  depth_attachment->samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

int create_framebuffers(struct VulkanRenderer* vulkan_renderer) {
  for (int loopNum = 0; loopNum < MAX_SWAP_CHAIN_FRAMES; loopNum++) {
    VkFramebufferCreateInfo framebufferInfo = {0};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vulkan_renderer->swapchain->render_pass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &vulkan_renderer->swapchain->swap_chain_image_views[loopNum];
    framebufferInfo.width = vulkan_renderer->swap_chain_extent.width;
    framebufferInfo.height = vulkan_renderer->swap_chain_extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(vulkan_renderer->device, &framebufferInfo, NULL, &vulkan_renderer->swapchain->swap_chain_framebuffers[loopNum]) != VK_SUCCESS)
      return VULKAN_RENDERER_CREATE_FRAME_BUFFER_ERROR;
  }

  return VULKAN_RENDERER_SUCCESS;
}

int create_command_pool(struct VulkanRenderer* vulkan_renderer) {
  VkCommandPoolCreateFlags command_pool_flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkCommandPoolCreateInfo pool_info = {0};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = vulkan_renderer->indices.graphics_family;
  pool_info.flags = command_pool_flags;

  if (vkCreateCommandPool(vulkan_renderer->device, &pool_info, NULL, &vulkan_renderer->command_pool) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_POOL_ERROR;

  return VULKAN_RENDERER_SUCCESS;
}

int create_command_buffers(struct VulkanRenderer* vulkan_renderer) {
  VkCommandBufferAllocateInfo alloc_unfo = {0};
  alloc_unfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_unfo.commandPool = vulkan_renderer->command_pool;
  alloc_unfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_unfo.commandBufferCount = (uint32_t)MAX_SWAP_CHAIN_FRAMES;

  if (vkAllocateCommandBuffers(vulkan_renderer->device, &alloc_unfo, vulkan_renderer->command_buffers) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  for (size_t i = 0; i < MAX_SWAP_CHAIN_FRAMES; i++) {
    command_buffer_start(vulkan_renderer, i);
    command_buffer_end(vulkan_renderer, i);
  }

  return VULKAN_RENDERER_SUCCESS;
}

int command_buffer_start(struct VulkanRenderer* vulkan_renderer, size_t i) {
  VkCommandBufferBeginInfo begin_info = {0};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  if (vkBeginCommandBuffer(vulkan_renderer->command_buffers[i], &begin_info) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  VkRenderPassBeginInfo render_pass_info = {0};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = vulkan_renderer->swapchain->render_pass;
  render_pass_info.framebuffer = vulkan_renderer->swapchain->swap_chain_framebuffers[i];
  render_pass_info.renderArea.offset.x = 0;
  render_pass_info.renderArea.offset.y = 0;
  render_pass_info.renderArea.extent = vulkan_renderer->swap_chain_extent;

  VkClearValue clear_values[2];
  memset(clear_values, 0, sizeof(clear_values));

  // http://ogldev.atspace.co.uk/www/tutorial51/tutorial51.html
  VkClearColorValue clear_color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clear_values[0].color = clear_color;

  VkClearDepthStencilValue depth_color = {1.0f, 0};
  clear_values[1].depthStencil = depth_color;

  render_pass_info.clearValueCount = 2;
  render_pass_info.pClearValues = clear_values;

  vkCmdBeginRenderPass(vulkan_renderer->command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

  return VULKAN_RENDERER_SUCCESS;
}

// TODO: Dump
int command_buffer_reset(struct VulkanRenderer* vulkan_renderer, size_t i) {
  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  if (vkResetCommandBuffer(vulkan_renderer->command_buffers[i], beginInfo.flags) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = vulkan_renderer->swapchain->render_pass;
  renderPassInfo.framebuffer = vulkan_renderer->swapchain->swap_chain_framebuffers[i];
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

  return VULKAN_RENDERER_SUCCESS;
}

int command_buffer_end(struct VulkanRenderer* vulkan_renderer, size_t i) {
  vkCmdEndRenderPass(vulkan_renderer->command_buffers[i]);

  if (vkEndCommandBuffer(vulkan_renderer->command_buffers[i]) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_COMMAND_BUFFER_ERROR;

  return VULKAN_RENDERER_SUCCESS;
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
      return VULKAN_RENDERER_CREATE_SYNC_OBJECT_ERROR;
  }

  return VULKAN_RENDERER_SUCCESS;
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

void recreate_swap_chain(struct VulkanRenderer* vulkan_renderer) {
  int width = 0, height = 0;
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(vulkan_renderer->glfw_window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(vulkan_renderer->device);

  vulkan_swap_chain_cleanup(vulkan_renderer);

  create_swap_chain(vulkan_renderer, width, height);
  //create_image_views(vulkan_renderer);
  //create_render_pass(vulkan_renderer);
  //create_graphics_pipeline(vulkan_renderer);
  //create_depth_resources(vulkan_renderer);
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
