#version 410

in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D uColorTexture;

void main(void)
{
    fragColor = texture(uColorTexture, vTexCoord);
}