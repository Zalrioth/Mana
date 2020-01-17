#include "mana/graphics/render/vulkanrenderer.h"

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  struct VulkanRenderer* vulkan_renderer_handle = (struct VulkanRenderer*)(glfwGetWindowUserPointer(window));
  vulkan_renderer_handle->framebuffer_resized = true;
}

int vulkan_renderer_init(struct VulkanRenderer* vulkan_renderer, int width, int height) {
  memset(vulkan_renderer, 0, sizeof(struct VulkanRenderer));
  vulkan_renderer->swap_chain = calloc(1, sizeof(struct SwapChain));
  vulkan_renderer->gbuffer = calloc(1, sizeof(struct GBuffer));
  vulkan_renderer->post_process = calloc(1, sizeof(struct PostProcess));

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  vulkan_renderer->glfw_window = glfwCreateWindow(width, height, "Grindstone", NULL, NULL);
  vulkan_renderer->framebuffer_resized = false;
  vulkan_renderer->physical_device = VK_NULL_HANDLE;

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
  // Good
  if ((error_code = create_surface(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_surface_error;
  if ((error_code = pick_physical_device(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_surface_error;
  // Vulkan errors
  if ((error_code = create_logical_device(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_device_error;
  if ((error_code = create_command_pool(vulkan_renderer)) != VULKAN_RENDERER_SUCCESS)
    goto vulkan_command_pool_error;

  swap_chain_init(vulkan_renderer->swap_chain, vulkan_renderer, width, height);
  post_process_init(vulkan_renderer->post_process, vulkan_renderer);
  gbuffer_init(vulkan_renderer->gbuffer, vulkan_renderer);

  // Maybe move this
  blit_swap_chain_init(vulkan_renderer->swap_chain->blit_swap_chain, vulkan_renderer);
  blit_post_process_init(vulkan_renderer->post_process->blit_post_process, vulkan_renderer);

  return VULKAN_RENDERER_SUCCESS;

vulkan_command_pool_error:
  vulkan_command_pool_cleanup(vulkan_renderer);
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
  post_process_delete(vulkan_renderer->post_process, vulkan_renderer);
  free(vulkan_renderer->post_process);
  gbuffer_delete(vulkan_renderer->gbuffer, vulkan_renderer);
  free(vulkan_renderer->gbuffer);
  swap_chain_delete(vulkan_renderer->swap_chain, vulkan_renderer);
  free(vulkan_renderer->swap_chain);
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
  VkApplicationInfo appInfo = {0};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Grindstone";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Mana";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo create_info = {0};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &appInfo;

  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  const char* converted_extensions[32];
  memset(converted_extensions, 0, sizeof(converted_extensions));

  for (int loop_num = 0; loop_num < glfw_extension_count; loop_num++)
    converted_extensions[loop_num] = glfw_extensions[loop_num];

  if (enable_validation_layers) {
    converted_extensions[glfw_extension_count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;  //"VK_EXT_DEBUG_UTILS_EXTENSION_NAME\0";
    glfw_extension_count++;
  }

  create_info.enabledExtensionCount = glfw_extension_count;
  create_info.ppEnabledExtensionNames = converted_extensions;

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

  struct Vector devices;
  memset(&devices, 0, sizeof(devices));
  vector_init(&devices, sizeof(VkPhysicalDevice));
  vector_resize(&devices, device_count);
  vkEnumeratePhysicalDevices(vulkan_renderer->instance, &device_count, devices.items);
  devices.size = device_count;

  VkPhysicalDeviceProperties current_device_properties = {0};
  uint32_t current_largest_heap = 0;
  bool discrete_selected = false;

  for (int loop_num = 0; loop_num < device_count; loop_num++) {
    VkPhysicalDevice device = *((VkPhysicalDevice*)vector_get(&devices, loop_num));
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);

    if (is_device_suitable(vulkan_renderer, device)) {
      if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        VkPhysicalDeviceMemoryProperties device_memory_properties = {0};
        vkGetPhysicalDeviceMemoryProperties(device, &device_memory_properties);
        for (int heap_num = 0; heap_num < device_memory_properties.memoryHeapCount; heap_num++) {
          VkMemoryHeap device_heap = device_memory_properties.memoryHeaps[heap_num];
          if (device_heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT && device_heap.size > current_largest_heap) {
            discrete_selected = true;
            current_largest_heap = device_heap.size;
            current_device_properties = device_properties;
            vulkan_renderer->physical_device = device;
          }
        }
      } else if (current_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        if (discrete_selected == false) {
          current_device_properties = device_properties;
          vulkan_renderer->physical_device = device;
        }
      } else
        printf("Unknown device: %s\n", device_properties.deviceName);
    }
  }

  printf("Selected device: %s\n", current_device_properties.deviceName);

  if (vulkan_renderer->physical_device == VK_NULL_HANDLE)
    return VULKAN_RENDERER_PICK_PHYSICAL_DEVICE_ERROR;

  vector_delete(&devices);

  return VULKAN_RENDERER_SUCCESS;
}

// TODO: Double check this
// Graphics: Can render something
// Present: Can draw to a screen
bool is_device_suitable(struct VulkanRenderer* vulkan_renderer, VkPhysicalDevice device) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

  struct Vector queue_families;
  memset(&queue_families, 0, sizeof(queue_families));
  vector_init(&queue_families, sizeof(VkQueueFamilyProperties));
  vector_resize(&queue_families, queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.items);
  queue_families.size = queue_family_count;

  bool graphics_family_found = false;
  bool present_family_found = false;

  //printf("%d", (*(VkQueueFamilyProperties*)vector_get(&queue_families, 0)).queueCount);

  int index_num = 0;
  for (int loop_num = 0; loop_num < queue_family_count; loop_num++) {
    if (((VkQueueFamilyProperties*)vector_get(&queue_families, loop_num))->queueCount > 0 && ((VkQueueFamilyProperties*)vector_get(&queue_families, loop_num))->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      (&vulkan_renderer->indices)->graphics_family = index_num;
      graphics_family_found = true;
    }
    VkBool32 present_support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, index_num, vulkan_renderer->surface, &present_support);

    if (((VkQueueFamilyProperties*)vector_get(&queue_families, loop_num))->queueCount > 0 && present_support) {
      (&vulkan_renderer->indices)->present_family = index_num;
      present_family_found = true;
    }

    if (graphics_family_found && present_family_found)
      break;

    index_num++;
  }

  if (!graphics_family_found || !present_family_found)
    return false;

  return true;
  // TODO: Should check for extension support
}

int create_logical_device(struct VulkanRenderer* vulkan_renderer) {
  // Note: Check this ma cause problems later
  struct Vector queue_create_infos;
  memset(&queue_create_infos, 0, sizeof(queue_create_infos));
  vector_init(&queue_create_infos, sizeof(struct VkDeviceQueueCreateInfo));

  float queue_priority = 1.0f;

  if (vulkan_renderer->indices.graphics_family == vulkan_renderer->indices.present_family) {
    VkDeviceQueueCreateInfo queue_create_infos_graphics = {0};
    queue_create_infos_graphics.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos_graphics.queueFamilyIndex = vulkan_renderer->indices.graphics_family;
    queue_create_infos_graphics.queueCount = 1;
    queue_create_infos_graphics.pQueuePriorities = &queue_priority;
    vector_push_back(&queue_create_infos, &queue_create_infos_graphics);
  } else {
    VkDeviceQueueCreateInfo queue_create_infos_graphics = {0};
    queue_create_infos_graphics.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos_graphics.queueFamilyIndex = vulkan_renderer->indices.graphics_family;
    queue_create_infos_graphics.queueCount = 1;
    queue_create_infos_graphics.pQueuePriorities = &queue_priority;
    vector_push_back(&queue_create_infos, &queue_create_infos_graphics);

    VkDeviceQueueCreateInfo queue_create_infos_present = {0};
    queue_create_infos_present.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos_present.queueFamilyIndex = vulkan_renderer->indices.present_family;
    queue_create_infos_present.queueCount = 1;
    queue_create_infos_present.pQueuePriorities = &queue_priority;
    vector_push_back(&queue_create_infos, &queue_create_infos_present);
  }

  struct VkPhysicalDeviceFeatures device_features = {0};
  device_features.samplerAnisotropy = VK_TRUE;

  struct VkDeviceCreateInfo device_info = {0};
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  device_info.queueCreateInfoCount = (uint32_t)queue_create_infos.size;
  device_info.pQueueCreateInfos = queue_create_infos.items;

  device_info.pEnabledFeatures = &device_features;

  device_info.enabledExtensionCount = (uint32_t)DEVICE_EXTENSION_COUNT;
  device_info.ppEnabledExtensionNames = device_extensions;

  if (enable_validation_layers) {
    device_info.enabledLayerCount = (uint32_t)VALIDATION_LAYER_COUNT;
    device_info.ppEnabledLayerNames = validation_layers;
  } else
    device_info.enabledLayerCount = 0;

  if (vkCreateDevice(vulkan_renderer->physical_device, &device_info, NULL, &vulkan_renderer->device) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_LOGICAL_DEVICE_ERROR;

  vkGetDeviceQueue(vulkan_renderer->device, vulkan_renderer->indices.graphics_family, 0, &vulkan_renderer->graphics_queue);
  vkGetDeviceQueue(vulkan_renderer->device, vulkan_renderer->indices.present_family, 0, &vulkan_renderer->present_queue);

  vector_delete(&queue_create_infos);

  return VULKAN_RENDERER_SUCCESS;
}

int create_image_view(struct VulkanRenderer* vulkan_renderer, struct VkImage_T* image, struct VkImageView_T* image_view) {
  VkImageViewCreateInfo view_info = {0};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view_info.format = vulkan_renderer->swap_chain->swap_chain_image_format;
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
  color_attachment->format = vulkan_renderer->swap_chain->swap_chain_image_format;
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

bool check_validation_layer_support() {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);

  struct Vector available_layers;
  memset(&available_layers, 0, sizeof(available_layers));
  vector_init(&available_layers, sizeof(VkLayerProperties));
  vector_resize(&available_layers, layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.items);
  available_layers.size = layer_count;

  bool layer_found = false;

  for (int loop_num = 0; loop_num < layer_count; loop_num++) {
    if (strcmp(validation_layers[0], ((VkLayerProperties*)vector_get(&available_layers, loop_num))->layerName) == 0) {
      layer_found = true;
      break;
    }
  }

  vector_delete(&available_layers);

  return layer_found;
}

void recreate_swap_chain(struct VulkanRenderer* vulkan_renderer) {
  int width = 0, height = 0;
  while (width == 0 || height == 0) {
    glfwGetFramebufferSize(vulkan_renderer->glfw_window, &width, &height);
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(vulkan_renderer->device);

  swap_chain_delete(vulkan_renderer->swap_chain, vulkan_renderer);

  //create_swap_chain(vulkan_renderer, width, height);
  //create_image_views(vulkan_renderer);
  //create_render_pass(vulkan_renderer);
  //create_graphics_pipeline(vulkan_renderer);
  //create_depth_resources(vulkan_renderer);
  //create_framebuffers(vulkan_renderer);
  //create_command_buffers(vulkan_renderer);
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
