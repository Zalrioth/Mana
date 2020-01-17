#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 outFragColor;

#define SHOW_EDGES 1
#define LUMA_THRESHOLD 0.5
#define MUL_REDUCE 8.0
#define MIN_REDUCE 128.0
#define MAX_SPAN 8.0

void main(void)
{
	vec2 textureDimensions = textureSize(texSampler, 0).xy;
	vec2 texelStep = vec2(1.0 / textureDimensions.x, 1.0 / textureDimensions.y);

  vec3 rgbM = texture(texSampler, inTexCoord).rgb;
	vec3 rgbNW = textureOffset(texSampler, inTexCoord, ivec2(-1, 1)).rgb;
  vec3 rgbNE = textureOffset(texSampler, inTexCoord, ivec2(1, 1)).rgb;
  vec3 rgbSW = textureOffset(texSampler, inTexCoord, ivec2(-1, -1)).rgb;
  vec3 rgbSE = textureOffset(texSampler, inTexCoord, ivec2(1, -1)).rgb;

	const vec3 toLuma = vec3(0.299, 0.587, 0.114);

	float lumaNW = dot(rgbNW, toLuma);
	float lumaNE = dot(rgbNE, toLuma);
	float lumaSW = dot(rgbSW, toLuma);
	float lumaSE = dot(rgbSE, toLuma);
	float lumaM = dot(rgbM, toLuma);

	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	if (lumaMax - lumaMin < lumaMax * LUMA_THRESHOLD) {
		outFragColor = vec4(rgbM, 1.0);
		return;
	}

	vec2 samplingDirection;
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

  float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * MUL_REDUCE, MIN_REDUCE);
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);

  samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-MAX_SPAN, -MAX_SPAN), vec2(MAX_SPAN, MAX_SPAN)) * texelStep;

	vec3 rgbSampleNeg = texture(texSampler, inTexCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(texSampler, inTexCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;
	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;
	vec3 rgbSampleNegOuter = texture(texSampler, inTexCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(texSampler, inTexCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;

	float lumaFourTab = dot(rgbFourTab, toLuma);
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
		outFragColor = vec4(rgbTwoTab, 1.0);
	else
		outFragColor = vec4(rgbFourTab, 1.0);

	if (SHOW_EDGES != 0)
		outFragColor.r = 1.0;
}