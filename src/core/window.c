#include "core/window.h"

// TODO: Condense functions
int window_init(struct Window *window, int width, int height) {
  memset(window, 0, sizeof(struct Window));

  window->width = width;
  window->height = height;

  int error_code;

  if ((error_code = init_vulkan_renderer(&window->renderer.vulkan_renderer, width, height)) != NO_ERROR)
    goto window_error;
  if ((error_code = create_instance(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto window_error;
  if ((error_code = setup_debug_messenger(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_debug_error;
  if ((error_code = create_surface(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_surface_error;
  if ((error_code = pick_physical_device(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_surface_error;
  if ((error_code = create_logical_device(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_device_error;
  if ((error_code = create_swap_chain(&window->renderer.vulkan_renderer, width, height)) != NO_ERROR)
    goto vulkan_swap_chain_error;
  if ((error_code = create_image_views(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_swap_chain_error;
  if ((error_code = create_render_pass(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_swap_chain_error;
  if ((error_code = create_descriptor_set_layout(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_desriptor_set_layout_error;
  if ((error_code = create_graphics_pipeline(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_desriptor_set_layout_error;
  if ((error_code = create_command_pool(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_command_pool_error;
  if ((error_code = create_depth_resources(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_command_pool_error;
  if ((error_code = create_framebuffers(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_command_pool_error;

  //
  if ((error_code = texture_create_image(&window->renderer.vulkan_renderer, window->renderer.vulkan_renderer.image_texture)) != NO_ERROR)
    goto vulkan_texture_error;
  if ((error_code = texture_create_texture_image_view(&window->renderer.vulkan_renderer, window->renderer.vulkan_renderer.image_texture)) != NO_ERROR)
    goto vulkan_texture_error;
  if ((error_code = texture_create_sampler(&window->renderer.vulkan_renderer, window->renderer.vulkan_renderer.image_texture)) != NO_ERROR)
    goto vulkan_texture_error;
  //

  if ((error_code = create_vertex_buffer(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_vertex_buffer_error;
  if ((error_code = create_index_buffer(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_index_buffer_error;
  if ((error_code = create_uniform_buffers(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_index_buffer_error;
  if ((error_code = create_descriptor_pool(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_index_buffer_error;
  if ((error_code = create_descriptor_sets(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_index_buffer_error;
  if ((error_code = create_command_buffers(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_index_buffer_error;
  if ((error_code = create_sync_objects(&window->renderer.vulkan_renderer)) != NO_ERROR)
    goto vulkan_sync_objects_error;

  return NO_ERROR;

vulkan_sync_objects_error:
  vulkan_sync_objects_cleanup(&window->renderer.vulkan_renderer);
vulkan_index_buffer_error:
  vulkan_index_buffer_cleanup(&window->renderer.vulkan_renderer);
vulkan_vertex_buffer_error:
  vulkan_vertex_buffer_cleanup(&window->renderer.vulkan_renderer);
vulkan_texture_error:
  vulkan_texture_cleanup(&window->renderer.vulkan_renderer);
vulkan_command_pool_error:
  vulkan_command_pool_cleanup(&window->renderer.vulkan_renderer);
vulkan_desriptor_set_layout_error:
  vulkan_descriptor_set_layout_cleanup(&window->renderer.vulkan_renderer);
vulkan_swap_chain_error:
  vulkan_swap_chain_cleanup(&window->renderer.vulkan_renderer);
vulkan_device_error:
  vulkan_device_cleanup(&window->renderer.vulkan_renderer);
vulkan_surface_error:
  vulkan_surface_cleanup(&window->renderer.vulkan_renderer);
vulkan_debug_error:
  vulkan_debug_cleanup(&window->renderer.vulkan_renderer);
window_error:
  window_cleanup(&window->renderer.vulkan_renderer);

  return error_code;
}

void window_delete(struct Window *window) {
  vulkan_renderer_delete(&window->renderer.vulkan_renderer);
}
