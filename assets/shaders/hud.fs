#version 330

in vec2 outTexCoord;
in vec3 mvPos;
out vec4 fragColor;

uniform sampler2D texture_sampler;
uniform vec4 colour;

//https://stackoverflow.com/questions/27827554/text-sharpness-while-using-fxaa
//http://hack.chrons.me/opengl-text-rendering/

void main()
{
    fragColor = colour * texture(texture_sampler, outTexCoord);
}