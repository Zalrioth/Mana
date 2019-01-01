#version 330

in vec2 outTexCoord;
in vec3 mvVertexNormal;
in vec3 mvVertexPos;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalColor;

//https://www.tomdalling.com/blog/modern-opengl/07-more-lighting-ambient-specular-attenuation-gamma/
//https://stackoverflow.com/questions/20940195/getting-world-position-for-deferred-rendering-light-pass

struct DirectionalLight
{
    vec3 colour;
    vec3 direction;
    float intensity;
};

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    int hasTexture;
    float reflectance;
};

uniform sampler2D texture_sampler;
uniform vec3 ambientLight;
uniform float specularPower;
uniform Material material;
uniform DirectionalLight directionalLight;

vec4 ambientC;
vec4 diffuseC;
vec4 speculrC;

void setupColours(Material material, vec2 textCoord)
{
    if (material.hasTexture == 1)
    {
        ambientC = texture(texture_sampler, textCoord);
        diffuseC = ambientC;
        speculrC = ambientC;
    }
    else
    {
        ambientC = material.ambient;
        diffuseC = material.diffuse;
        speculrC = material.specular;
    }
}

vec4 calcLightColour(vec3 light_colour, float light_intensity, vec3 position, vec3 to_light_dir, vec3 normal)
{
    vec4 diffuseColour = vec4(0, 0, 0, 0);
    vec4 specColour = vec4(0, 0, 0, 0);

    // Diffuse Light
    float diffuseFactor = max(dot(normal, to_light_dir), 0.0);
    diffuseColour = diffuseC * vec4(light_colour, 1.0) * light_intensity * diffuseFactor;

    // Specular Light
    vec3 camera_direction = normalize(-position);
    vec3 from_light_dir = -to_light_dir;
    vec3 reflected_light = normalize(reflect(from_light_dir , normal));
    float specularFactor = max( dot(camera_direction, reflected_light), 0.0);
    specularFactor = pow(specularFactor, specularPower);
    specColour = speculrC * light_intensity  * specularFactor * material.reflectance * vec4(light_colour, 1.0);

    return (diffuseColour + specColour);
}

vec4 calcDirectionalLight(DirectionalLight light, vec3 position, vec3 normal)
{
    return calcLightColour(light.colour, light.intensity, position, normalize(light.direction), normal);
}

//https://stackoverflow.com/questions/27997900/how-to-get-a-value-from-vec3-in-vertex-shader-opengl-3-3
//http://www.fundza.com/rman_shaders/smoothstep/
//http://prideout.net/blog/?p=22
//http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/toon-shading-version-iii/
//https://www.opengl.org/discussion_boards/showthread.php/181863-How-to-render-normals-to-texture-via-FBO
//https://www.opengl.org/discussion_boards/showthread.php/183955-gl_NormalMatrix-gl_Normal-are-deprecated

const float NEAR = 0.01f;
const float FAR = 1000;
/*float linearizeDepth(float depth)
{
    return (2.0 * NEAR) / (FAR + NEAR - depth * (FAR - NEAR));
}*/

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    normalColor = vec4(normalize(mvVertexNormal), 1.0);

    setupColours(material, outTexCoord);

    vec4 diffuseSpecularComp = calcDirectionalLight(directionalLight, mvVertexPos, mvVertexNormal);

    fragColor = ambientC * vec4(ambientLight, 1) + diffuseSpecularComp;
    //normalColor = vec4(mvVertexNormal.x * 0.5 + 0.5, mvVertexNormal.y * 0.5 + 0.5, mvVertexNormal.z * 0.5 + 0.5, 1.0);


    //float intensity = dot(diffuseSpecularComp.rgb, vec3(0.299, 0.587, 0.144));

    //float intensity = dot(diffuseSpecularComp.rgb, normalize(normalColor.rgb));

    /*float cutoff1 = 0.95;
    float cutoff2 = 0.5;
    float cutoff3 = 0.25;

    float smoothLimit = 0.02;

    float shade1 = 1;
    float shade2 = 0.6;
    float shade3 = 0.4;
    float shade4 = 0.2;

    if (intensity > cutoff1) {
        diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade1;
    }
    else if (intensity > cutoff2) {
      	diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade2;
    }
    else if (intensity > cutoff3) {
        diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade3;
    }
    else {
        diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade4;
    }

    fragColor = ambientC * vec4(ambientLight, 1) + diffuseSpecularComp;*/

    /*float cutoff1 = 0.95;
    float cutoff2 = 0.75;
    float cutoff3 = 0.4;

    float smoothLimit = 0.02;

    float shade1 = 1;
    float shade2 = 0.8;
    float shade3 = 0.6;

    // Could use smoothstep to enforce a sorta gradient but no full look at sword art online reference image of running

    if (intensity > cutoff1) {
        diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade1;
    }
    //else if (intensity > cutoff1 - smoothLimit) {
        //diffuseSpecularComp.rgb = directionalLight.colour.rgb * (smoothstep(cutoff1 - smoothLimit, cutoff1, intensity) / (shade1 / shade2)) + (shade1 - shade2);
    //}
    else if (intensity > cutoff2) {
      	diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade1;
    }
    //else if (intensity > cutoff2 - smoothLimit) {
        //diffuseSpecularComp.rgb = directionalLight.colour.rgb * (smoothstep(cutoff2 - smoothLimit, cutoff2, intensity) / (shade2 / shade3)) + (shade2 - shade3);
    //}
    else if (intensity > cutoff3) {
          	diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade2;
        }
    else {
        diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade3;
    }

    fragColor = ambientC * vec4(ambientLight, 1) + diffuseSpecularComp;*/
    //fragColor = ambientC * vec4(ambientLight, 1) * diffuseSpecularComp;
    //fragColor = ambientC * (vec4(ambientLight, 1) / vec4(ambientLight, 1)) - (1-diffuseSpecularComp);
    //fragColor = ambientC - (1-diffuseSpecularComp);
    //fragColor = ambientC;

    /*if (intensity > cutoff1) {
        diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade1;
    }
    else if (intensity > cutoff2) {
      	diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade2;
    }
    else {
        diffuseSpecularComp.rgb = directionalLight.colour.rgb * shade3;
    }*/

    //fragColor = ambientC * vec4(ambientLight, 1) + diffuseSpecularComp;

    //normalColor = vec4(mvVertexNormal.x * 0.5 + 0.5, mvVertexNormal.y * 0.5 + 0.5, mvVertexNormal.z * 0.5 + 0.5, 1.0);

    /*if (intensity > cutoff1) {
        diffuseSpecularComp.rgb = vec3(shade1 * directionalLight.colour.r, shade1 * directionalLight.colour.g, shade1 * directionalLight.colour.b);
    }
    else if (intensity > cutoff2) {
      	diffuseSpecularComp.rgb = vec3(shade2 * directionalLight.colour.r, shade2 * directionalLight.colour.g, shade2 * directionalLight.colour.b);
    }
    else {
        diffuseSpecularComp.rgb = vec3(shade3 * directionalLight.colour.r, shade3 * directionalLight.colour.g, shade3 * directionalLight.colour.b);
    }

    fragColor = ambientC * vec4(ambientLight, 1) + diffuseSpecularComp;*/

    //float intensity = diffuseSpecularComp.r + diffuseSpecularComp.g + diffuseSpecularComp.b;

    // I can't think of a good solulation may have to hard code ramps for sunset and what not

    /*for (int loopNum = 0; loopNum < 4; loopNum++)
    {
        if (diffuseSpecularComp[loopNum] > 0.95) {
        	diffuseSpecularComp[loopNum] = 1;
        }
        else if (diffuseSpecularComp[loopNum] > 0.5) {
        	diffuseSpecularComp[loopNum] = 0.4;
        }
       else {
           	diffuseSpecularComp[loopNum] = 0.15;
        }
    }*/

    // current
    /*if (diffuseSpecularComp.r > 0.95) {
        diffuseSpecularComp.r = 0.95;
    }
    else if (diffuseSpecularComp.r > 0.25) {
      	diffuseSpecularComp.r = 0.5;
    }
    else {
        diffuseSpecularComp.r = 0.2;
    }

    fragColor = ambientC * vec4(ambientLight, 1) + diffuseSpecularComp.r;*/

    /*vec3 intensity = diffuseSpecularComp.rgb;

    if (intensity > 0.95)
    {
        intensity = 0.95;
    }
    else if (intensity > 0.7)
    {
        intensity = 0.75;
    }
    else if (intensity > 0.5)
    {
        intensity = 0.55;
    }
    else
    {
       	intensity = 0.25;
    }

    fragColor = ambientC * vec4(ambientLight, 1) + intensity;*/
}

/*layout (binding = 0) uniform sampler1D tex_toon;

uniform vec3 light_pos = vec3(30.0, 30.0, 100.0);

in VS_OUT
{
    vec3 normal;
    vec3 view;
} fs_in;

out vec4 color;

void main(void)
{
    // Calculate per-pixel normal and light vector
    vec3 N = normalize(fs_in.normal);
    vec3 L = normalize(light_pos - fs_in.view);

    // Simple N dot L diffuse lighting
    float tc = pow(max(0.0, dot(N, L)), 5.0);

    // Sample from cell shading texture
    color = texture(tex_toon, tc) * (tc * 0.8 + 0.2);
}*/