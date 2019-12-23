#include "graphics/shader.h"

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

int shader_init(struct Shader* shader, struct VulkanRenderer* vulkan_renderer, char* vertex_shader, char* fragment_shader, char* geometry_shader) {
  // Get the current working directory
#if defined(IS_WINDOWS)
  char* buffer;
  buffer = _getcwd(NULL, 0);
  printf("%s \nLength: %llu\n", buffer, strlen(buffer));
#else
  char cwd[LARGE_BUFFER];
    getcwd(cwd, sizeof(cwd);
    printf("Current working dir: %s\n", cwd);
#endif

  int vertex_length = 0;
  int fragment_length = 0;

  char* vert_shader_code = read_shader_file(vertex_shader, &vertex_length);
  char* frag_shader_code = read_shader_file(fragment_shader, &fragment_length);

  VkShaderModule vert_shader_module = shader_create_shader_module(vulkan_renderer, vert_shader_code, vertex_length);
  VkShaderModule frag_shader_module = shader_create_shader_module(vulkan_renderer, frag_shader_code, fragment_length);

  VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
  vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_shader_stage_info.module = vert_shader_module;
  vert_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
  frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_shader_stage_info.module = frag_shader_module;
  frag_shader_stage_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

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

  //VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
  //depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  //depth_stencil.depthTestEnable = VK_TRUE;
  //depth_stencil.depthWriteEnable = VK_TRUE;
  //depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
  //depth_stencil.depthBoundsTestEnable = VK_FALSE;
  //depth_stencil.stencilTestEnable = VK_FALSE;

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
  pipeline_layout_info.pSetLayouts = &shader->descriptor_set_layout;

  if (vkCreatePipelineLayout(vulkan_renderer->device, &pipeline_layout_info, NULL, &shader->pipeline_layout) != VK_SUCCESS)
    return 0;

  VkGraphicsPipelineCreateInfo pipelineInfo = {0};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shader_stages;
  pipelineInfo.pVertexInputState = &vertex_input_info;
  pipelineInfo.pInputAssemblyState = &input_assembly;
  pipelineInfo.pViewportState = &viewport_state;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  //pipelineInfo.pDepthStencilState = &depth_stencil;
  pipelineInfo.pColorBlendState = &color_blending;
  pipelineInfo.layout = shader->pipeline_layout;
  pipelineInfo.renderPass = vulkan_renderer->swapchain->render_pass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(vulkan_renderer->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &shader->graphics_pipeline) != VK_SUCCESS)
    return VULKAN_RENDERER_CREATE_GRAPHICS_PIPELINE_ERROR;

  vkDestroyShaderModule(vulkan_renderer->device, frag_shader_module, NULL);
  vkDestroyShaderModule(vulkan_renderer->device, vert_shader_module, NULL);

  return VULKAN_RENDERER_SUCCESS;
}

void shader_delete(struct Shader* shader, struct VulkanRenderer* vulkan_renderer) {
  vkDestroyPipeline(vulkan_renderer->device, shader->graphics_pipeline, NULL);
  vkDestroyPipelineLayout(vulkan_renderer->device, shader->pipeline_layout, NULL);
}

VkShaderModule shader_create_shader_module(struct VulkanRenderer* vulkan_renderer, const char* code, int length) {
  VkShaderModuleCreateInfo create_info = {0};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = length;
  create_info.pCode = (const uint32_t*)(code);

  VkShaderModule shader_module;
  if (vkCreateShaderModule(vulkan_renderer->device, &create_info, NULL, &shader_module) != VK_SUCCESS)
    fprintf(stderr, "Failed to create shader module!\n");

  return shader_module;
}
