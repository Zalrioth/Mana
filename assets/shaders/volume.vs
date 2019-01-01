#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;

// uniform float t;
// uniform float radius_world;

uniform mat4 P, M, V;

// out float isovalue;
//out vec3 fragNor;
//out vec2 xyPos;
out vec3 mvVertexNormal;
out vec3 mvVertexPos;

void main() {
    // // World center is (0, 0), so I don't need to include it in the equations.
    // vec2 direction = -vertPos;
    // float distToCenter = length(vertPos);

    // // Find the t that will put this point on the edge of the circle.
    // float maxT = (distToCenter - radius_world) / length(direction);
    // vec2 destination = vertPos + maxT * direction;

    // // If the point moves past the edge of the circle, set its position to the edge.
    // if (t > maxT) {
    //     gl_Position = vec4(destination, 0, 1.0);
    // } else {
    //     gl_Position = vec4(vertPos + t*direction, 0, 1.0);
    // }

    vec4 mvPos = V * M * vec4(vertPos.xyz, 1.0);
    gl_Position = P * mvPos;
    mvVertexNormal = normalize(vertNor);
    mvVertexPos = mvPos.xyz;
    //fragNor = normalize(vertNor);
    //xyPos = vertPos.xy;
    // isovalue = vertAttrs.w;
}