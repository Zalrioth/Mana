#include "mana/core/vulkancore.h"

int vulkan_core_init(struct VulkanState* vulkan_state) {
  vulkan_state->framebuffer_resized = false;
  vulkan_state->physical_device = VK_NULL_HANDLE;

  int error_code;
  if ((error_code = vulkan_core_create_instance(vulkan_state)) != VULKAN_CORE_SUCCESS)
    return -1;
  if ((error_code = vulkan_core_setup_debug_messenger(vulkan_state)) != VULKAN_CORE_SUCCESS)
    goto vulkan_debug_error;
  if ((error_code = vulkan_core_pick_physical_device(vulkan_state)) != VULKAN_CORE_SUCCESS)
    goto vulkan_surface_error;
  if ((error_code = vulkan_core_create_logical_device(vulkan_state)) != VULKAN_CORE_SUCCESS)
    goto vulkan_device_error;
  if ((error_code = vulkan_core_create_command_pool(vulkan_state)) != VULKAN_CORE_SUCCESS)
    goto vulkan_command_pool_error;

vulkan_command_pool_error:
//    vulkan_command_pool_cleanup(vulkan_state);
vulkan_device_error:
//  vulkan_device_cleanup(vulkan_state);
vulkan_surface_error:
//  vulkan_surface_cleanup(vulkan_state);
vulkan_debug_error:
  //  vulkan_debug_cleanup(vulkan_state);

  return error_code;
}

void vulkan_command_pool_cleanup(struct VulkanState* vulkan_state) {
  vkDestroyCommandPool(vulkan_state->device, vulkan_state->command_pool, NULL);
}

static void vulkan_core_destroy_debug_utils_messenger_ext(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* p_allocator) {
  PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != NULL)
    func(instance, debug_messenger, p_allocator);
}

int vulkan_core_create_instance(struct VulkanState* vulkan_state) {
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

  if (vkCreateInstance(&create_info, NULL, &vulkan_state->instance) != VK_SUCCESS)
    return VULKAN_CORE_CREATE_INSTANCE_ERROR;

  return VULKAN_CORE_SUCCESS;
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

int vulkan_core_setup_debug_messenger(struct VulkanState* vulkan_state) {
  if (enable_validation_layers && !vulkan_core_check_validation_layer_support(vulkan_state))
    return VULKAN_CORE_SETUP_DEBUG_MESSENGER_ERROR;

  if (enable_validation_layers) {
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {0};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = debug_callback;

    if (create_debug_utils_messenger_ext(vulkan_state->instance, &debugInfo, NULL, &vulkan_state->debug_messenger) != VK_SUCCESS)
      return VULKAN_CORE_SETUP_DEBUG_MESSENGER_ERROR;
  }

  return VULKAN_CORE_SUCCESS;
}

int vulkan_core_pick_physical_device(struct VulkanState* vulkan_state) {
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(vulkan_state->instance, &device_count, NULL);

  if (device_count == 0)
    return VULKAN_CORE_PICK_PHYSICAL_DEVICE_ERROR;

  struct Vector devices;
  memset(&devices, 0, sizeof(devices));
  vector_init(&devices, sizeof(VkPhysicalDevice));
  vector_resize(&devices, device_count);
  vkEnumeratePhysicalDevices(vulkan_state->instance, &device_count, devices.items);
  devices.size = device_count;

  VkPhysicalDeviceProperties current_device_properties = {0};
  uint32_t current_largest_heap = 0;
  bool discrete_selected = false;

  for (int loop_num = 0; loop_num < device_count; loop_num++) {
    VkPhysicalDevice device = *((VkPhysicalDevice*)vector_get(&devices, loop_num));
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);

    if (vulkan_core_can_device_render(vulkan_state, device)) {
      if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        VkPhysicalDeviceMemoryProperties device_memory_properties = {0};
        vkGetPhysicalDeviceMemoryProperties(device, &device_memory_properties);
        for (int heap_num = 0; heap_num < device_memory_properties.memoryHeapCount; heap_num++) {
          VkMemoryHeap device_heap = device_memory_properties.memoryHeaps[heap_num];
          if (device_heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT && device_heap.size > current_largest_heap) {
            discrete_selected = true;
            current_largest_heap = device_heap.size;
            current_device_properties = device_properties;
            vulkan_state->physical_device = device;
          }
        }
      } else if (current_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        if (discrete_selected == false) {
          current_device_properties = device_properties;
          vulkan_state->physical_device = device;
        }
      } else
        printf("Unknown device: %s\n", device_properties.deviceName);
    }
  }

  printf("Selected device: %s\n", current_device_properties.deviceName);

  if (vulkan_state->physical_device == VK_NULL_HANDLE)
    return VULKAN_CORE_PICK_PHYSICAL_DEVICE_ERROR;

  vector_delete(&devices);

  return VULKAN_CORE_SUCCESS;
}

bool vulkan_core_can_device_render(struct VulkanState* vulkan_state, VkPhysicalDevice device) {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

  struct Vector queue_families = {0};
  memset(&queue_families, 0, sizeof(queue_families));
  vector_init(&queue_families, sizeof(VkQueueFamilyProperties));
  vector_resize(&queue_families, queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.items);
  queue_families.size = queue_family_count;

  bool graphics_family_found = false;

  int index_num = 0;
  for (int loop_num = 0; loop_num < queue_family_count; loop_num++) {
    if (((VkQueueFamilyProperties*)vector_get(&queue_families, loop_num))->queueCount > 0 && ((VkQueueFamilyProperties*)vector_get(&queue_families, loop_num))->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      (&vulkan_state->indices)->graphics_family = index_num;
      graphics_family_found = true;
    }

    if (graphics_family_found)
      break;

    index_num++;
  }

  if (!graphics_family_found)
    return false;

  return true;
  // TODO: Should check for extension support
}

int vulkan_core_create_logical_device(struct VulkanState* vulkan_state) {
  // Note: Check this may cause problems later
  // TODO: Redo below
  struct Vector queue_create_infos;
  memset(&queue_create_infos, 0, sizeof(queue_create_infos));
  vector_init(&queue_create_infos, sizeof(struct VkDeviceQueueCreateInfo));

  float queue_priority = 1.0f;
  if (vulkan_state->indices.graphics_family == vulkan_state->indices.present_family) {
    VkDeviceQueueCreateInfo queue_create_infos_graphics = {0};
    queue_create_infos_graphics.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos_graphics.queueFamilyIndex = vulkan_state->indices.graphics_family;
    queue_create_infos_graphics.queueCount = 1;
    queue_create_infos_graphics.pQueuePriorities = &queue_priority;
    vector_push_back(&queue_create_infos, &queue_create_infos_graphics);
  } else {
    VkDeviceQueueCreateInfo queue_create_infos_graphics = {0};
    queue_create_infos_graphics.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos_graphics.queueFamilyIndex = vulkan_state->indices.graphics_family;
    queue_create_infos_graphics.queueCount = 1;
    queue_create_infos_graphics.pQueuePriorities = &queue_priority;
    vector_push_back(&queue_create_infos, &queue_create_infos_graphics);

    VkDeviceQueueCreateInfo queue_create_infos_present = {0};
    queue_create_infos_present.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos_present.queueFamilyIndex = vulkan_state->indices.present_family;
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

  if (vkCreateDevice(vulkan_state->physical_device, &device_info, NULL, &vulkan_state->device) != VK_SUCCESS)
    return VULKAN_CORE_CREATE_LOGICAL_DEVICE_ERROR;

  vkGetDeviceQueue(vulkan_state->device, vulkan_state->indices.graphics_family, 0, &vulkan_state->graphics_queue);
  vkGetDeviceQueue(vulkan_state->device, vulkan_state->indices.present_family, 0, &vulkan_state->present_queue);

  vector_delete(&queue_create_infos);

  return VULKAN_CORE_SUCCESS;
}

VkSampleCountFlagBits vulkan_core_get_max_usable_sample_count(struct VulkanState* vulkan_state) {
  return VK_SAMPLE_COUNT_1_BIT;
  VkPhysicalDeviceProperties physical_device_properties;
  vkGetPhysicalDeviceProperties(vulkan_state->physical_device, &physical_device_properties);

  VkSampleCountFlags counts = physical_device_properties.limits.framebufferColorSampleCounts & physical_device_properties.limits.framebufferDepthSampleCounts;
  if (counts & VK_SAMPLE_COUNT_64_BIT)
    return VK_SAMPLE_COUNT_64_BIT;
  if (counts & VK_SAMPLE_COUNT_32_BIT)
    return VK_SAMPLE_COUNT_32_BIT;
  if (counts & VK_SAMPLE_COUNT_16_BIT)
    return VK_SAMPLE_COUNT_16_BIT;
  if (counts & VK_SAMPLE_COUNT_8_BIT)
    return VK_SAMPLE_COUNT_8_BIT;
  if (counts & VK_SAMPLE_COUNT_4_BIT)
    return VK_SAMPLE_COUNT_4_BIT;
  if (counts & VK_SAMPLE_COUNT_2_BIT)
    return VK_SAMPLE_COUNT_2_BIT;

  return VK_SAMPLE_COUNT_1_BIT;
}

int vulkan_core_create_command_pool(struct VulkanState* vulkan_state) {
  VkCommandPoolCreateFlags command_pool_flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  VkCommandPoolCreateInfo pool_info = {0};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = vulkan_state->indices.graphics_family;
  pool_info.flags = command_pool_flags;

  if (vkCreateCommandPool(vulkan_state->device, &pool_info, NULL, &vulkan_state->command_pool) != VK_SUCCESS)
    return VULKAN_CORE_CREATE_COMMAND_POOL_ERROR;

  return VULKAN_CORE_SUCCESS;
}

bool vulkan_core_check_validation_layer_support(struct VulkanState* vulkan_state) {
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
