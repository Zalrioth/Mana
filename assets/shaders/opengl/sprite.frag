#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 normalColor;

#define Z_NEAR 10000000.0f
#define Z_FAR 0.0f

float linearDepth(float depth) {
	return (2.0f * Z_NEAR) / (Z_FAR + Z_NEAR - depth * (Z_FAR - Z_NEAR));	
}

void main() {
    //outColor = texture(texSampler, fragTexCoord);
    // NOTE: Final channel will probably be used for something
    //outColor = vec4(normalize(inNormal) * 0.5 + 0.5, 1.0) * 0.5 + texture(texSampler, fragTexCoord) * 0.5;
    //outColor = vec4(normalize(inNormal), 1.0);
    //outColor = vec4(WorldPosFromDepth(gl_FragCoord.z), 1.0);
    //float depth = linearDepth(gl_FragCoord.z);
    //float depth = gl_FragCoord.z;
    //outColor = vec4(vec3(depth), 1.0) + texture(texSampler, fragTexCoord) * 0.5;
    outColor = vec4(vec3(1.0), 1.0);
    //outColor = vec4(normalize(outNormal), 1.0) * 0.5 + texture(texSampler, fragTexCoord) * 0.5;
    //normalColor = vec4(normalize(outNormal), 1.0);
    normalColor = vec4(normalize(inNormal), 1.0);
}
