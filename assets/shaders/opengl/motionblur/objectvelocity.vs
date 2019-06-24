#version 410

out vec4 vPosition;
out vec4 vPrevPosition;

layout (location=0) in vec3 position;

uniform mat4 uModelViewMat;
uniform mat4 uPrevModelViewMat;
uniform mat4 projectionMatrix;

void main()
{
    vPosition = projectionMatrix * uModelViewMat * vec4(position, 1.0);;
    vPrevPosition = projectionMatrix * uPrevModelViewMat * vec4(position, 1.0);;

    gl_Position = vPosition;
}