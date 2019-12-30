#pragma once
#ifndef POST_PROCESS_H
#define POST_PROCESS_H

#include <stdbool.h>

struct PostProcess {
  struct VkFramebuffer_T* post_process_framebuffers[2];

  struct VkImage_T* color_images[2];
  struct VkDeviceMemory_T* color_image_memories[2];
  struct VkImageView_T* color_views[2];

  bool ping_pong;
};

#endif  // POST_PROCESS_H