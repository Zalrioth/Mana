#include "mana/graphics/entities/planet.h"

void planet_init(struct Planet* planet, struct GPUAPI* gpu_api, size_t octree_size, struct Shader* shader, vec3 position, struct Vector* noises, float (*density_func_single)(struct Vector*, float, float, float), float* (*density_func_set)(struct Vector*, float, float, float, int, int, int)) {
  planet->planet_type = ROUND_PLANET;
  planet->terrain_shader = shader;
  planet->position = position;
  dual_contouring_init(&planet->dual_contouring, gpu_api, octree_size, shader, noises, density_func_single, density_func_set);
}

void planet_delete(struct Planet* planet, struct GPUAPI* gpu_api) {
  dual_contouring_delete(&planet->dual_contouring, gpu_api);
}

void planet_render(struct Planet* planet, struct GPUAPI* gpu_api) {
  vkCmdBindPipeline(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, planet->terrain_shader->graphics_pipeline);
  VkBuffer vertex_buffers[] = {planet->dual_contouring.vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, planet->dual_contouring.index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, planet->terrain_shader->pipeline_layout, 0, 1, &planet->dual_contouring.descriptor_set, 0, NULL);
  vkCmdDrawIndexed(gpu_api->vulkan_state->gbuffer->gbuffer_command_buffer, planet->dual_contouring.mesh->indices->size, 1, 0, 0, 0);
}

// TODO: Pass lights and sun position?
void planet_update_uniforms(struct Planet* planet, struct GPUAPI* gpu_api, struct Camera* camera, vec3 light_pos) {
  struct DualContouringUniformBufferObject dcubo = {{{0}}};
  dcubo.proj = gpu_api->vulkan_state->gbuffer->projection_matrix;
  dcubo.view = gpu_api->vulkan_state->gbuffer->view_matrix;
  dcubo.proj.m11 *= -1;
  dcubo.model = MAT4_IDENTITY;
  dcubo.model = mat4_translate(dcubo.model, planet->position);
  dcubo.camera_pos = camera->position;
  void* terrain_data;
  vkMapMemory(gpu_api->vulkan_state->device, planet->dual_contouring.dc_uniform_buffer_memory, 0, sizeof(struct DualContouringUniformBufferObject), 0, &terrain_data);
  memcpy(terrain_data, &dcubo, sizeof(struct DualContouringUniformBufferObject));
  vkUnmapMemory(gpu_api->vulkan_state->device, planet->dual_contouring.dc_uniform_buffer_memory);

  struct LightingUniformBufferObject light_ubo = {{0}};
  light_ubo.direction = light_pos;
  light_ubo.ambient_color = (vec3){.data[0] = 1.0f, .data[1] = 1.0f, .data[2] = 1.0f};
  light_ubo.diffuse_colour = (vec3){.data[0] = 1.0f, .data[1] = 1.0f, .data[2] = 1.0f};
  light_ubo.specular_colour = (vec3){.data[0] = 1.0f, .data[1] = 1.0f, .data[2] = 1.0f};

  void* lighting_data;
  vkMapMemory(gpu_api->vulkan_state->device, planet->dual_contouring.lighting_uniform_buffer_memory, 0, sizeof(struct LightingUniformBufferObject), 0, &lighting_data);
  memcpy(lighting_data, &light_ubo, sizeof(struct LightingUniformBufferObject));
  vkUnmapMemory(gpu_api->vulkan_state->device, planet->dual_contouring.lighting_uniform_buffer_memory);
}
