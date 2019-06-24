#version 410

in vec2 vTexCoord;
layout(location = 0) out vec4 fragColor;

uniform sampler2D uColorTexture;

void main(void)
{
    fragColor = texture(uColorTexture, vTexCoord);
    //if (vTexCoord.x > 0.5 && vTexCoord.y > 0.5)
        //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}