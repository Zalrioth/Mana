#version 410

in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D depthImage;

//http://www.geeks3d.com/20091216/geexlab-how-to-visualize-the-depth-buffer-in-glsl/
//http://glampert.com/2014/01-26/visualizing-the-depth-buffer/

float LinearizeDepth(in vec2 uv)
{
    //float zNear = 1.0;
    //float zFar  = 100.0;
    float zNear = 0.01;
    float zFar  = 1000.0;
    //float zNear = 0.1;
    //float zFar  = 1000.0;
    float depth = texture(depthImage, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
    //float depth = texture(depthImage, uv).r * 2.0 - 1.0;
    //return (2.0 * zNear * zFar) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
    float c = LinearizeDepth(vTexCoord);
    fragColor = vec4(c, c, c, 1.0);
}