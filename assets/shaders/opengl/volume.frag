#version 410 core
// uniform vec2 screenCenter;
// uniform float radius_pixel;
// out vec4 color;

// in float isovalue;
//in vec3 fragNor;
in vec3 mvVertexNormal;
in vec3 mvVertexPos;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalColor;
layout(location = 2) out vec4 positionColor;
layout(location = 3) out vec4 lightScatteringColor;

void main()
{
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

    normalColor = vec4(mvVertexNormal, 1.0);

    if (normalColor.g > 0.5)
        fragColor = vec4(0.376, 0.502, 0.22, 1.0);//fragColor = vec4(0.75,1,0.2,1);
    else
        fragColor = vec4(0.5, 0.5, 0.5, 1.0);

    fragColor += 0.25;

    positionColor = vec4(mvVertexPos, 1.0);
    // Add transparency if applicable
    lightScatteringColor = vec4(0.0, 0.0, 0.0, 1.0);

    //float linearDepth = LinearizeDepth(mvVertexPos);
    //linearDepthColor = vec4(linearDepth, linearDepth, linearDepth, 1.0);

    //positionColor = vec4(VSPositionFromDepth(linearDepthColor.r), 1.0);

    //color = vec4(mvVertexNormal, 1);

    // if (abs(isovalue) < 1) {
         //color = vec4(1, 1, 1, 1);
    // } else {
    //     discard;
    // }
    //color = vec4(normalize(fragNor), 1);
}