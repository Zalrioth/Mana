#version 410 core

uniform sampler2D hdr_image;

uniform vec2 direction;

in vec2 vTexCoord;
out vec4 color;

//vec2 texelStep = vec2(1.0 / 600.0, 1.0 / 480.0);
//vec2 texelStep = vec2(1.0 / 300.0, 1.0 / 240.0);

const float weights[] = float[](0.0024499299678342,
                                0.0043538453346397,
                                0.0073599963704157,
                                0.0118349786570722,
                                0.0181026699707781,
                                0.0263392293891488,
                                0.0364543006660986,
                                0.0479932050577658,
                                0.0601029809166942,
                                0.0715974486241365,
                                0.0811305381519717,
                                0.0874493212267511,
                                0.0896631113333857,
                                0.0874493212267511,
                                0.0811305381519717,
                                0.0715974486241365,
                                0.0601029809166942,
                                0.0479932050577658,
                                0.0364543006660986,
                                0.0263392293891488,
                                0.0181026699707781,
                                0.0118349786570722,
                                0.0073599963704157,
                                0.0043538453346397,
                                0.0024499299678342);

void main(void)
{
    vec4 c = vec4(0.0);
    /*ivec2 P = ivec2(vTexCoord.yx) - ivec2(0, weights.length() >> 1);
    int i;

    for (i = 0; i < weights.length(); i++)
    {
        c += texelFetch(hdr_image, P + ivec2(0, i), 0) * weights[i];
    }*/

    /*int i;

    for (i = 0; i < weights.length(); i++)
    {
        c += texture(hdr_image, vTexCoord + vec2(0, float(i) * texelStep.y)) * weights[i];
    }*/

    /*int i;

    vec2 P = vTexCoord - vec2(0, float(weights.length() >> 1) * texelStep.y);

    for (i = 0; i < weights.length(); i++)
    {
        c += texture(hdr_image, P + vec2(0, float(i) * texelStep.y)) * weights[i];
    }*/

    vec2 texelSize = 1.0 / vec2(textureSize(hdr_image, 0));
    vec2 texelStep = texelSize * 2;

    int i;

    vec2 P = vTexCoord - vec2(float(weights.length() >> 1) * texelStep.x * direction.x, float(weights.length() >> 1) * texelStep.y * direction.y);

    for (i = 0; i < weights.length(); i++)
    {
        c += texture(hdr_image, P + vec2(float(i) * texelStep.x * direction.x, float(i) * texelStep.y * direction.y)) * weights[i];
    }

    color = c;
    //color = vec4(1,1,1,1);

    //color = texture(hdr_image, vTexCoord);
}