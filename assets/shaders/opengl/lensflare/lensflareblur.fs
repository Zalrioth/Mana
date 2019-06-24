#version 410 core

in vec2 vTexCoord;
layout(location = 0) out vec3 fragColor;

uniform sampler2D uColorTexture;

uniform int direction;

//uniform vec2 resolution;
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
    vec3 c = vec3(0.0);

    //vec2 texelStep = vec2(1.0 / resolution.x, 1.0 / resolution.y);
    //vec2 texelStep = vec2(1.0 / resolution.x, 1.0 / resolution.y);
    vec2 texelStep = 1.0 / vec2(textureSize(uColorTexture, 0));
    //texelStep *= 2;
    //vec2 texelStep = texelSize * 2;
    
    if (direction == 0) {
        vec2 P = vTexCoord - vec2(float(weights.length() >> 1) * texelStep.x, 0);

        for (int i = 0; i < weights.length(); i++)
            c += texture(uColorTexture, P + vec2(float(i) * texelStep.x, 0)).rgb * weights[i];

        fragColor = c;
    }
    else {
        //vec2 P = vTexCoord - vec2(float(weights.length() >> 1) * texelStep.x * direction.x, float(weights.length() >> 1) * texelStep.y * direction.y);
        vec2 P = vTexCoord - vec2(0, float(weights.length() >> 1) * texelStep.y);

        //c += texture(uColorTexture, P + vec2(float(i) * texelStep.x * direction.x, float(i) * texelStep.y * direction.y)) * weights[i];
        for (int i = 0; i < weights.length(); i++)
            c += texture(uColorTexture, P + vec2(0, float(i) * texelStep.y)).rgb * weights[i];

        fragColor = c;
    }
}