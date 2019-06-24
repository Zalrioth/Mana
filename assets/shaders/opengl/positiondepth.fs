#version 410 core

//https://learnopengl.com/Advanced-OpenGL/Depth-testing

in vec2 vTexCoord;

layout(location = 2) out vec4 positionColor;

uniform sampler2D gDepthTexture;
uniform mat4 invProjMatrix;

float NEAR = 0.01;
float FAR = 1000;

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    float depth = texture(gDepthTexture, vTexCoord).r;
    float linearDepth = linearizeDepth(depth) / FAR;

    positionColor.z = linearDepth;
    //linearDepthColor = vec4(vec3(linearDepth), 1.0);
    //positionColor = vec4(VSPositionFromDepth(linearDepth), 1.0);
    //positionColor.z = linearDepth;
}