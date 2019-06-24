//https://www.opengl.org/discussion_boards/showthread.php/173900-motion-blur-post-effect-shader-in-GLSL
//http://john-chapman-graphics.blogspot.com/2013/01/per-object-motion-blur.html
//http://john-chapman-graphics.blogspot.com/2013/01/what-is-motion-blur-motion-pictures-are.html
//http://ogldev.atspace.co.uk/www/tutorial41/tutorial41.html
//https://stevenlu.net/files/motion_blur_2d/Fragment_shader_dynamic_blur.pdf

#version 410

uniform mat4 viewProjectionInverse;
uniform mat4 oldViewProjection;

uniform sampler2D depthTexture;

in vec2 vTexCoord;
out vec4 fragColor;

// NOTE: All depth data is stored in zOverW red
void main(void)
{
    //vec4 color = texture(colorTexture, vTexCoord);
    float zOverW = texture(depthTexture, vTexCoord).r;
    vec4 H = vec4(vTexCoord.x * 2 - 1, (1 - vTexCoord.y) * 2 - 1, zOverW, 1);
    vec4 D = viewProjectionInverse * H;
    vec4 worldPos = D / vec4(D.w);

    vec4 currentPos = H;
    vec4 previousPos = oldViewProjection * worldPos;
    previousPos = previousPos / vec4(previousPos.w);
    vec2 velocity = vec2(currentPos.xy - previousPos.xy)/2.0;

    fragColor = vec4(velocity.x, velocity.y, 0.0, 1.0);
    //fragColor = vec4(velocity.x, velocity.y, color.b, 1.0);
}




























/*#version 410

uniform mat4 viewProjectionInverse;
uniform mat4 oldViewProjection;

uniform sampler2D depthTexture;
uniform sampler2D colorTexture;

in vec2 vTexCoord;

out vec4 fragColor;

//out vec2 velocity;

// NOTE: All depth data is stored in zOverW red

void main(void)
{
    vec4 color = texture(colorTexture, vTexCoord);
    vec4 zOverW = texture(depthTexture, vTexCoord);
    vec4 H = vec4(vTexCoord.x * 2 - 1, (1 - vTexCoord.y) * 2 - 1, zOverW.g, 1);
    vec4 D = viewProjectionInverse * H;
    vec4 worldPos = D / vec4(D.w);

    vec4 currentPos = H;
    vec4 previousPos = oldViewProjection * worldPos;
    previousPos = previousPos / vec4(previousPos.w);
    vec2 velocity = vec2(currentPos.xy - previousPos.xy)/2.0;

    fragColor = vec4(velocity.x, velocity.y, color.g, 1.0);
}*/



























/*#version 410

uniform mat4 oldProjection;
uniform mat4 newProjection;
uniform mat4 oldView;
uniform mat4 newView;
uniform sampler2D depthTexture;

in vec2 v_texCoord;

//out vec2 velocity;

void main()
{
    float zOverW = texture(depthTexture, v_texCoord).r;
    vec4 H = vec4(v_texCoord.x * 2 - 1, (1 - v_texCoord.y) * 2 - 1, zOverW, 1);
    vec4 D = H * (newProjection * newView).inverse;
    vec4 worldPos = D / D.w;

    vec4 currentPos = H;
    vec4 previousPos = worldPos * (oldProjection * oldView);
    previousPos /= previousPos.w;
    vec2 velocity = vec2(currentPos - previousPos)/2.f;
    gl_FragColor = (currentPos - previousPos)/2.f;
}*/

/*#version 330

in vec2 speed;

vec3 getSpeedColor()
{
  return vec3(0.5 + 0.5 * speed, 0.);
}

void main()
{
  gl_FragData[0] = 0.;
  gl_FragData[1] = getSpeedColor();
}*/


























/*#version 330

smooth in vec4 vPosition;
smooth in vec4 vPrevPosition;

out vec2 oVelocity;

void main(void) {
   vec2 a = (vPosition.xy / vPosition.w) * 0.5 + 0.5;
   vec2 b = (vPrevPosition.xy / vPrevPosition.w) * 0.5 + 0.5;
   oVelocity = a - b;
}*/

/*varying vec2 speed;
vec3 getSpeedColor()
{
  return vec3(0.5 + 0.5 * speed, 0.);
}

void main()
{
  gl_FragData[0] = 0.;
  gl_FragData[1] = getSpeedColor();
}*/

/*uniform sampler2D uTexLinearDepth;

uniform mat4 uInverseModelViewMat; // inverse model->view
uniform mat4 uPrevModelViewProj; // previous model->view->projection

noperspective in vec2 TexCoord0;
noperspective in vec3 vViewRay; // for extracting current world space position

void main() {
   // get current world space position:
   vec3 current = vViewRay * texture(uTexLinearDepth, TexCoord0).r;
   current = uInverseModelViewMat * current;

   // get previous screen space position:
   vec4 previous = uPrevModelViewProj * vec4(current, 1.0);
   previous.xyz /= previous.w;
   previous.xy = previous.xy * 0.5 + 0.5;

   vec2 blurVec = previous.xy - TexCoord0;
}*/

/*#version 410

smooth in vec4 vPosition;
smooth in vec4 vPrevPosition;

out vec2 oVelocity;

void main(void) {
   vec2 a = (vPosition.xy / vPosition.w) * 0.5 + 0.5;
   vec2 b = (vPrevPosition.xy / vPrevPosition.w) * 0.5 + 0.5;
   oVelocity = a - b;
}*/