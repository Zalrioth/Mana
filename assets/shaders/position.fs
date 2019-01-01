#version 410

in vec2 v_texCoord;
out vec4 fragColor;

uniform sampler2D u_depthTexture;

void main(void)
{
    float depth = getLinearDepth(texture(u_depthTexture, v_texCoord).rgb);

    vec4 pos = vec4(v_texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    pos = uInverseViewProjectionMatrix * pos;
    pos /= pos.w;
}