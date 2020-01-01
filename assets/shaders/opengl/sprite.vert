#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitTangent;

layout(location = 0) out vec3 mvVertexNormal;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(ubo.view * ubo.model)));
	mvVertexNormal = normalMatrix * inNormal;

    //mvVertexNormal = (ubo.view * ubo.model * vec4(inNormal, 0.0)).xyz;
    // Normal multiplied by model matrix
    // https://github.com/SaschaWillems/Vulkan/blob/master/data/shaders/mesh/mesh.vert
    //outNormal = mat3(ubo.model) * inNormal;
    //outNormal = mat3(transpose(inverse(ubo.view))) * inNormal;
    fragTexCoord = inTexCoord;
}
