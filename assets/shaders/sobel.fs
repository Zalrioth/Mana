//https://en.wikipedia.org/wiki/Cel_shading
//https://computergraphics.stackexchange.com/questions/3646/opengl-glsl-sobel-edge-detection-filter/3647
//https://github.com/spite/Wagner/blob/master/fragment-shaders/sobel2-fs.glsl
//https://computergraphics.stackexchange.com/questions/2450/opengl-detection-of-edges
//https://www.gamedev.net/forums/topic/650930-edge-detection-with-normaldepth-and-sobel/
//https://computergraphics.stackexchange.com/questions/3646/opengl-glsl-sobel-edge-detection-filter
//https://stackoverflow.com/questions/13836597/outline-shader-with-variable-width-lines?rq=1
//http://homepages.inf.ed.ac.uk/rbf/HIPR2/sobel.htm
//https://computergraphics.stackexchange.com/questions/3807/sobel-edge-detection-line-thickness

#version 330 core

in vec2 vTexCoord;
out vec4 color;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);

//float xPixel = 1.0/600.0;
//float yPixel = 1.0/480.0;

void main()
{
    //fragColor = texture(uColorTexture, vTexCoord);
    vec4 diffuse = texture(colorTexture, vTexCoord);
    //vec3 diffuse = texture(normalTexture, vTexCoord.st).rgb;

    float depthOffset = texture(depthTexture, vTexCoord).r;

    //float division = (1.0 - sqrt(sqrt(depthOffset)));
    float division = (1.0 - pow(depthOffset, 1.0 / 10.0));

    vec2 texelSize = 1.0 / vec2(textureSize(normalTexture, 0));

    float xPixel = texelSize.x;
    float yPixel = texelSize.y;

    mat3 I;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec3 sample = texture(normalTexture, vTexCoord - vec2((i - 1) * xPixel * division, (j - 1) * yPixel * division)).rgb;
            //vec3 sample = texelFetch(normalTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0).rgb;
            //vec3 sample = texelFetch(normalTexture, ivec2(gl_FragCoord) + texelFetch(depthTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0).rgb;
            I[i][j] = length(sample);
        }
    }

    float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]);
    float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

    float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));

    color = diffuse - vec4(vec3(g), 1.0);

    /*mat3 J;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            //vec3 sample = texelFetch(depthTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0).rgb;
            vec3 sample = texture(depthTexture, vTexCoord - vec2((i - 1) * xPixel * division, (j - 1) * yPixel * division)).rgb;
            J[i][j] = length(sample);
        }
    }

    float gx2 = dot(sx[0], J[0]) + dot(sx[1], J[1]) + dot(sx[2], J[2]);
    float gy2 = dot(sy[0], J[0]) + dot(sy[1], J[1]) + dot(sy[2], J[2]);

    float g2 = sqrt(pow(gx2, 2.0)+pow(gy2, 2.0));

    color -= vec4(vec3(g2), 1.0);*/
}