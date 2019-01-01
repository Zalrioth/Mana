#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture;
uniform sampler2D gMotionTexture;

float uVelocityScale = 1.0;

//float uVelocityScale = 144.0/60.0;

const int MAX_SAMPLES = 32;

void main(void) {
    vec2 texelSize = 1.0 / vec2(textureSize(gColorTexture, 0));
    //vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

    //vec2 velocity = texture(gMotionTexture, screenTexCoords).rg;// * 2.0 - 1.0;
    vec2 velocity = texture(gMotionTexture, v_texCoord).rg;
    velocity *= uVelocityScale;

    float speed = length(velocity / texelSize);
    int nSamples = clamp(int(speed), 1, MAX_SAMPLES);

    //vec4 oResult = texture(gColorTexture, screenTexCoords);
    vec4 oResult = texture(gColorTexture, v_texCoord);
    for (int i = 1; i < nSamples; ++i)
    {
        //vec2 offset = velocity * (float(i) / float(nSamples));//(float(i) / float(nSamples - 1) - 0.5);
        vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
        oResult += texture(gColorTexture, v_texCoord + offset);
    }
    oResult /= float(nSamples);

    FragColor = oResult;
    FragColor.a = 1.0f;
}























/*#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture; // texture we're blurring
uniform sampler2D gMotionTexture; // velocity buffer

//float uVelocityScale = 60.0/144.0;
float uVelocityScale = 2.0;

void main(void) {
    vec2 texelSize = 1.0 / vec2(textureSize(gColorTexture, 0));
    vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

    vec2 velocity = texture(gMotionTexture, screenTexCoords).rg;
    velocity *= uVelocityScale;

    float speed = length(velocity / texelSize);

    vec4 oResult = texture(gColorTexture, screenTexCoords);
    for (int i = 1; i < 32; ++i)
    {
        vec2 offset = velocity * (float(i) / float(32 - 1) - 0.5);
        oResult += texture(gColorTexture, screenTexCoords + offset);
    }
    oResult /= float(32);

    FragColor = oResult;
}*/

/*#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture; // texture we're blurring
uniform sampler2D gMotionTexture; // velocity buffer

//float uVelocityScale = 60.0/144.0;
float uVelocityScale = 0.2;

void main(void) {
    vec4 output = texture(gColorTexture, v_texCoord);

    vec4 holder = texture(gMotionTexture, v_texCoord);

    output += holder;

    FragColor = output;
}*/







/*#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture; // texture we're blurring
uniform sampler2D gMotionTexture; // velocity buffer

//float uVelocityScale = 60.0/144.0;
float uVelocityScale = 0.2;

void main(void) {
    vec2 texelSize = 1.0 / vec2(textureSize(gColorTexture, 0));
    vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

    vec2 velocity = texture(gMotionTexture, screenTexCoords).rg;
    velocity *= uVelocityScale;

    float speed = length(velocity / texelSize);

    vec4 oResult = texture(gColorTexture, screenTexCoords);
    for (int i = 1; i < 32; ++i)
    {
        vec2 offset = velocity * (float(i) / float(32 - 1) - 0.5);
        oResult += texture(gColorTexture, screenTexCoords + offset);
    }
    oResult /= float(32);

    FragColor = oResult;
}*/


















/*#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture; // texture we're blurring
uniform sampler2D gMotionTexture; // velocity buffer

const int MAX_SAMPLES = 32;

void main(void) {
    vec4 result = texture(gColorTexture, v_texCoord);
    vec2 blurVec = texture(gMotionTexture, v_texCoord).rg;

    //vec2 blurVec = vec2(texture(gMotionTexture, v_texCoord).r, texture(gMotionTexture, v_texCoord).g);

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        vec2 offset = blurVec * (float(i) / float(MAX_SAMPLES - 1) - 0.5);
        result += texture(gColorTexture, v_texCoord + offset);
    }

   result /= float(MAX_SAMPLES);

   FragColor = result;
}*/






































/*#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture; // texture we're blurring
uniform sampler2D gMotionTexture; // velocity buffer

float uVelocityScale = 1.0;

const int MAX_SAMPLES = 32;

void main(void) {
    vec2 texelSize = 1.0 / vec2(textureSize(gColorTexture, 0));
    vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

    vec2 velocity = texture(gMotionTexture, screenTexCoords).rg;
    velocity *= uVelocityScale;

    float speed = length(velocity / texelSize);
    int nSamples = clamp(int(speed), 1, MAX_SAMPLES);

    vec4 oResult = texture(gColorTexture, screenTexCoords);
    for (int i = 1; i < nSamples; ++i)
    {
        vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
        oResult += texture(gColorTexture, screenTexCoords + offset);
    }
    oResult /= float(nSamples);

    FragColor = oResult;
}*/

/*#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture; // texture we're blurring
uniform sampler2D gMotionTexture; // velocity buffer

float uVelocityScale = 1.0;

const int MAX_SAMPLES = 1;

void main(void) {
    vec2 texelSize = 1.0 / vec2(textureSize(gColorTexture, 0));
    vec2 texCoord = v_texCoord * texelSize;

    vec2 velocity = texture(gMotionTexture, texCoord).rg;

    texCoord += velocity;

    float speed = length(velocity / texelSize);
    int nSamples = clamp(int(speed), 1, MAX_SAMPLES);

    vec4 oResult = texture(gColorTexture, texCoord);
    for (int i = 1; i < nSamples; ++i)
    {
        vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
        oResult += texture(gColorTexture, texCoord + offset);
    }
    oResult /= float(nSamples);

    FragColor = oResult;*/

    /*for(int i = 1; i < g_numSamples; ++i, texCoord += velocity)
    {
        vec4 currentColor = texture(gColorTexture, texCoord);
        color += currentColor;
    }

    FragColor = color / g_numSamples;*/
//}

/*#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture; // texture we're blurring
uniform sampler2D gMotionTexture; // velocity buffer

float uVelocityScale = 1.0;

void main(void) {
    vec2 texelSize = 1.0 / vec2(textureSize(gColorTexture, 0));
    vec2 screenTexCoords = gl_FragCoord.xy * texelSize;

    vec2 velocity = texture(gMotionTexture, screenTexCoords).rg;
    velocity *= uVelocityScale;

    float speed = length(velocity / texelSize);

    vec4 oResult = texture(gColorTexture, screenTexCoords);
    for (int i = 1; i < 32; ++i)
    {
        vec2 offset = velocity * (float(i) / float(32 - 1) - 0.5);
        oResult += texture(gColorTexture, screenTexCoords + offset);
    }
    oResult /= float(32);

    FragColor = oResult;
}*/

/*#version 410

in vec2 v_texCoord;
out vec4 FragColor;

uniform sampler2D gColorTexture;
uniform sampler2D gMotionTexture;

void main()
{
    vec2 MotionVector = texture(gMotionTexture, v_texCoord).xy / 2.0;

    vec4 Color = vec4(0.0);

    vec2 TexCoord = v_texCoord;

    Color += texture(gColorTexture, TexCoord) * 0.4;
    TexCoord -= MotionVector;
    Color += texture(gColorTexture, TexCoord) * 0.3;
    TexCoord -= MotionVector;
    Color += texture(gColorTexture, TexCoord) * 0.2;
    TexCoord -= MotionVector;
    Color += texture(gColorTexture, TexCoord) * 0.1;

    FragColor = Color;
}*/

/*#version 330

in vec2 TexCoord0;

uniform sampler2D gColorTexture;
uniform sampler2D gMotionTexture;

out vec4 FragColor;

void main()
{
    vec2 MotionVector = texture(gMotionTexture, TexCoord0).xy / 2.0;

    vec4 Color = vec4(0.0);

    vec2 TexCoord = TexCoord0;

    Color += texture(gColorTexture, TexCoord) * 0.4;
    TexCoord -= MotionVector;
    Color += texture(gColorTexture, TexCoord) * 0.3;
    TexCoord -= MotionVector;
    Color += texture(gColorTexture, TexCoord) * 0.2;
    TexCoord -= MotionVector;
    Color += texture(gColorTexture, TexCoord) * 0.1;

    FragColor = Color;
}*/