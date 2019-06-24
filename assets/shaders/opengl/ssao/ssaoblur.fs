#version 410 core

in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D colorTexture;
uniform sampler2D ssaoTexture;

float object_level = 1.0;
float ssao_level = 1.0;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, vTexCoord + offset).r;
        }
    }

    vec4 object_color = texture(colorTexture, vTexCoord);

    result = result / (4.0 * 4.0);

    fragColor = vec4(object_color.rgb * result, 1.0);
}