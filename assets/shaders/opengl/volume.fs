#version 330 core
// uniform vec2 screenCenter;
// uniform float radius_pixel;
// out vec4 color;

// in float isovalue;
in vec3 mvVertexNormal;
in vec3 mvVertexPos;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalColor;

//https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
//https://gamedev.stackexchange.com/questions/53272/how-to-texture-a-surface-generated-by-marching-cubes-algorithm?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
//https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/mod.xhtml

struct DirectionalLight
{
    vec3 colour;
    vec3 direction;
    float intensity;
};

uniform vec3 ambientLight;
uniform float specularPower;
uniform DirectionalLight directionalLight;

vec4 ambientC;
vec4 diffuseC;
vec4 speculrC;
float reflectance;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
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
    specColour = speculrC * light_intensity  * specularFactor * reflectance * vec4(light_colour, 1.0);

    return (diffuseColour + specColour);
}

vec4 calcDirectionalLight(DirectionalLight light, vec3 position, vec3 normal)
{
    return calcLightColour(light.colour, light.intensity, position, normalize(light.direction), normal);
}

void main()
{

    //diffuseC = vec4(0.3,0.3,0.3,0.3);
    //speculrC = vec4(0.2,0.2,0.2,0.2);
    //diffuseC = vec4(1.0,1.0,1.0,1.0);
    speculrC = vec4(0,0,0,0.1);
    reflectance = 0.1;

    // original working normal
    //normalColor = vec4(normalize(fragNor), 1);
    //color = vec4(fragNor, 1);

    normalColor = vec4(mvVertexNormal, 1);

    if (normalColor.g > 0.5)
        ambientC = vec4(0.75,1,0.2,1);
    else
        ambientC = vec4(0.5,0.5,0.5,1.0);

    diffuseC = ambientC;

    vec4 diffuseSpecularComp = calcDirectionalLight(directionalLight, mvVertexPos, mvVertexNormal);

    fragColor = ambientC * vec4(ambientLight, 1) + diffuseSpecularComp;

        fragColor *= 0.001;
        fragColor += normalColor;

    // // gl_PointCoord ranges from 0-1, thus the center of the point would be (0.5, 0.5)
    // vec2 pointCenter = vec2(0.5, 0.5);
    // float dist = distance(gl_PointCoord, pointCenter);

    // if (dist > 0.5) {
    //     discard;
    // } else {
    //     if (distance(gl_FragCoord, vec4(screenCenter.x, screenCenter.y, 0, 0)) < radius_pixel) {
    //         color = vec4(0.5, 0.75, 1.0, 0.8) - vec4(dist*1.5, dist*1.5, dist*1.5, 0.0);
    //     } else {
    //         color = vec4(1.0, 0.75, 0.5, 0.4) - vec4(dist*1.5, dist*1.5, dist*1.5, 0.0);
    //     }
    // }

    // if (abs(isovalue) < 1) {
    //     color = vec4(1, 1, 1, 1);
    // } else {
    //     discard;
    // }

    //color *= 1.0 + mod(rand(xyPos.xy), 0.5);
}