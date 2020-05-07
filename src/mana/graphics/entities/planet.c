#include "mana/graphics/entities/planet.h"

void planet_init(struct Planet* planet, struct VulkanState* vulkan_renderer, size_t octree_size, struct Shader* shader, struct Vector* noises, float (*density_func_single)(struct Vector*, float, float, float), float* (*density_func_set)(struct Vector*, float, float, float, int, int, int)) {
  planet->planet_type = ROUND_PLANET;
  planet->terrain_shader = shader;
  dual_contouring_init(&planet->dual_contouring, vulkan_renderer, octree_size, shader, noises, density_func_single, density_func_set);
}

void planet_delete(struct Planet* planet, struct VulkanState* vulkan_renderer) {
  dual_contouring_delete(&planet->dual_contouring, vulkan_renderer);
}

void planet_render(struct Planet* planet, struct VulkanState* vulkan_renderer) {
  vkCmdBindPipeline(vulkan_renderer->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, planet->terrain_shader->graphics_pipeline);
  VkBuffer vertex_buffers[] = {planet->dual_contouring.vertex_buffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(vulkan_renderer->gbuffer->gbuffer_command_buffer, 0, 1, vertex_buffers, offsets);
  vkCmdBindIndexBuffer(vulkan_renderer->gbuffer->gbuffer_command_buffer, planet->dual_contouring.index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdBindDescriptorSets(vulkan_renderer->gbuffer->gbuffer_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, planet->terrain_shader->pipeline_layout, 0, 1, &planet->dual_contouring.descriptor_set, 0, NULL);
  vkCmdDrawIndexed(vulkan_renderer->gbuffer->gbuffer_command_buffer, planet->dual_contouring.mesh->indices->size, 1, 0, 0, 0);
}

// TODO: Pass lights and sun position?
void planet_update_uniforms(struct Planet* planet, struct VulkanState* vulkan_renderer, struct Camera* camera, vec3 light_pos) {
  struct DualContouringUniformBufferObject dcubo = {{{0}}};
  glm_mat4_copy(vulkan_renderer->gbuffer->projection_matrix, dcubo.proj);
  glm_mat4_copy(vulkan_renderer->gbuffer->view_matrix, dcubo.view);
  dcubo.proj[1][1] *= -1;
  glm_mat4_identity(dcubo.model);
  glm_vec3_copy(camera->position, dcubo.camera_pos);
  void* terrain_data;
  vkMapMemory(vulkan_renderer->device, planet->dual_contouring.dc_uniform_buffer_memory, 0, sizeof(struct DualContouringUniformBufferObject), 0, &terrain_data);
  memcpy(terrain_data, &dcubo, sizeof(struct DualContouringUniformBufferObject));
  vkUnmapMemory(vulkan_renderer->device, planet->dual_contouring.dc_uniform_buffer_memory);

  struct LightingUniformBufferObject light_ubo = {{0}};
  glm_vec3_copy(light_pos, light_ubo.direction);
  vec3 light_ambient = {1.0f, 1.0f, 1.0f};
  glm_vec3_copy(light_ambient, light_ubo.ambient_color);
  vec3 light_diffuse = {1.0f, 1.0f, 1.0f};
  glm_vec3_copy(light_diffuse, light_ubo.diffuse_colour);
  vec3 light_specular = {1.0f, 1.0f, 1.0f};
  glm_vec3_copy(light_specular, light_ubo.specular_colour);

  void* lighting_data;
  vkMapMemory(vulkan_renderer->device, planet->dual_contouring.lighting_uniform_buffer_memory, 0, sizeof(struct LightingUniformBufferObject), 0, &lighting_data);
  memcpy(lighting_data, &light_ubo, sizeof(struct LightingUniformBufferObject));
  vkUnmapMemory(vulkan_renderer->device, planet->dual_contouring.lighting_uniform_buffer_memory);
}
