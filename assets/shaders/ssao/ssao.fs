#version 410

//https://github.com/Zalrioth/OpenGL/blob/master/Example28/shader/ssao.frag.glsl
//https://github.com/maesse/CubeTech/blob/master/src/cubetech/gfx/SSAO.java
//http://www.java-gaming.org/index.php?topic=37715.0
//http://john-chapman-graphics.blogspot.co.uk/2013/01/ssao-tutorial.html
//http://devmaster.net/p/3095/shader-effects-screen-space-ambient-occlusion
//https://stackoverflow.com/questions/30383310/opengl-ogldev-ssao-tutorial-implementation-fragment-shader-yields-noise

// tabs open
//https://github.com/maesse/CubeTech/blob/c6ca3358526feb7e8fb2a8d4f1eefb11c3e6b1cd/src/cubetech/gfx/SSAO.java
//http://www.java-gaming.org/index.php?topic=37715.0
//https://learnopengl.com/#!Advanced-Lighting/SSAO
//https://gamedev.stackexchange.com/questions/106503/ssao-depth-buffer-linearization
//https://stackoverflow.com/questions/30383310/opengl-ogldev-ssao-tutorial-implementation-fragment-shader-yields-noise
//https://github.com/ocharles/SSAO-example/blob/master/resources/shaders/ssao_fs.glsl
//https://github.com/ocharles/SSAO-example/blob/master/resources/shaders/ssao_fs.glsl
//https://pastebin.com/a3Jb2wCT
//https://mynameismjp.wordpress.com/2009/03/10/reconstructing-position-from-depth/
//https://stackoverflow.com/questions/4899555/glsl-how-to-get-pixel-x-y-z-world-position
//https://stackoverflow.com/questions/14317350/transform-world-space-normals-to-screen-space-normals
//https://stackoverflow.com/questions/9062116/opengl-glsl-ssao-implementation
//https://stackoverflow.com/questions/18776791/opengl-gbuffer-normals-and-depth-buffer-issues
//https://stackoverflow.com/questions/9062116/opengl-glsl-ssao-implementation

// Getting world position actually works
//https://stackoverflow.com/questions/32227283/getting-world-position-from-depth-buffer-value

// Faster to calculate position than fetch from memory
//https://stackoverflow.com/questions/20940195/getting-world-position-for-deferred-rendering-light-pass

// Full source
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/9.ssao/ssao.cpp


in vec2 vTexCoord;
out float fragColor;

uniform sampler2D u_texture;
uniform sampler2D gNormals;
uniform sampler2D gLinearDepth;
uniform sampler2D noiseTexture;
//uniform sampler2D positionTexture;

uniform vec3 samples[16];
uniform mat4 projMatrix;
uniform mat4 invProjMatrix;
uniform mat4 invViewMatrix;

//const vec2 noiseScale = vec2(600.0/4.0, 480.0/4.0);

//uniform vec2 noiseScale;

int kernelSize = 16;
//float radius = 0.5;
float radius = 1.0;
float bias = 0.025;

float NEAR = 0.01;
float FAR = 1000;

vec3 VSPositionFromDepth(in float depth)
{
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(vTexCoord * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProjMatrix * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    //if (z > 0.9999)
        //viewSpacePosition.xyz = vec3(0.0);

    return viewSpacePosition.xyz;
}

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main(void)
{
    vec2 noiseScale = vec2(textureSize(gNormals, 0)) / 4.0;

    vec3 fragPos     = VSPositionFromDepth(texture(gLinearDepth, vTexCoord).r);
    vec3 normal    = texture(gNormals, vTexCoord).xyz;
    vec3 randomVec = texture(noiseTexture, vTexCoord * noiseScale).xyz;

    normal         = normalize(normal);
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = normalize(cross(normal, tangent));
    //vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    float depthHere = linearizeDepth(texture(gLinearDepth, vTexCoord).r);

    float occlusion = 0.0;

    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samp = TBN * samples[i]; // From tangent to view-space
        samp = fragPos + samp * radius;

        vec4 offset = vec4(samp, 1.0);
        offset      = projMatrix * offset;    // from view to clip-space
        offset.xyz /= offset.w;               // perspective divide
        offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        float sampleDepth = linearizeDepth(texture(gLinearDepth, offset.xy).r);

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(depthHere - sampleDepth));
        //float rangeCheck = abs(depthHere - sampleDepth) > radius ? 1.0 : 0.0;
        occlusion += (sampleDepth <= depthHere - bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    //fragColor = vec4(vec3(pow(occlusion, 8)), 1.0);
    fragColor = occlusion;//vec4(vec3(occlusion), 1.0);
    //fragColor = pow(occlusion, 2);
}
