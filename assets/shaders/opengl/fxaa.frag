#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D tex_sampler;

layout(location = 0) in vec2 in_tex_coord;

layout(location = 0) out vec4 out_frag_color;

//#define SHOW_EDGES
#define LUMA_THRESHOLD 0.1 // 0.5
#define MUL_REDUCE (1.0 / 8.0)
#define MIN_REDUCE (1.0 / 128.0)
#define MAX_SPAN 8.0

void main(void)
{
	ivec2 texture_dimensions = textureSize(tex_sampler, 0).xy;
	vec2 texel_step = vec2(1.0 / texture_dimensions.x, 1.0 / texture_dimensions.y);

	vec3 rgb_m = texture(tex_sampler, in_tex_coord).rgb;
	vec3 rgb_nw = textureOffset(tex_sampler, in_tex_coord, ivec2(-1, 1)).rgb;
	vec3 rgb_ne = textureOffset(tex_sampler, in_tex_coord, ivec2(1, 1)).rgb;
	vec3 rgb_sw = textureOffset(tex_sampler, in_tex_coord, ivec2(-1, -1)).rgb;
	vec3 rgb_se = textureOffset(tex_sampler, in_tex_coord, ivec2(1, -1)).rgb;

	const vec3 to_luma = vec3(0.299, 0.587, 0.114);

	float luma_nw = dot(rgb_nw, to_luma);
	float luma_ne = dot(rgb_ne, to_luma);
	float luma_sw = dot(rgb_sw, to_luma);
	float luma_se = dot(rgb_se, to_luma);
	float luma_m = dot(rgb_m, to_luma);

	float luma_min = min(luma_m, min(min(luma_nw, luma_ne), min(luma_sw, luma_se)));
	float luma_max = max(luma_m, max(max(luma_nw, luma_ne), max(luma_sw, luma_se)));

	if (luma_max - luma_min < luma_max * LUMA_THRESHOLD) {
		out_frag_color = vec4(rgb_m, 1.0);
		return;
	}

	vec2 sampling_direction = vec2(-((luma_nw + luma_ne) - (luma_sw + luma_se)), ((luma_nw + luma_sw) - (luma_ne + luma_se)));

	float sampling_direction_reduce = max((luma_nw + luma_ne + luma_sw + luma_se) * 0.25 * MUL_REDUCE, MIN_REDUCE);
	float minsampling_directionFactor = 1.0 / (min(abs(sampling_direction.x), abs(sampling_direction.y)) + sampling_direction_reduce);

	sampling_direction = clamp(sampling_direction * minsampling_directionFactor, vec2(-MAX_SPAN, -MAX_SPAN), vec2(MAX_SPAN, MAX_SPAN)) * texel_step;

	vec3 rgb_sample_neg = texture(tex_sampler, in_tex_coord + sampling_direction * (1.0 / 3.0 - 0.5)).rgb;
	vec3 rgb_sample_pos = texture(tex_sampler, in_tex_coord + sampling_direction * (2.0 / 3.0 - 0.5)).rgb;
	vec3 rgb_two_tab = (rgb_sample_pos + rgb_sample_neg) * 0.5;
	vec3 rgb_sample_negOuter = texture(tex_sampler, in_tex_coord + sampling_direction * (0.0 / 3.0 - 0.5)).rgb;
	vec3 rgb_sample_posOuter = texture(tex_sampler, in_tex_coord + sampling_direction * (3.0 / 3.0 - 0.5)).rgb;
	vec3 rgb_four_tab = (rgb_sample_posOuter + rgb_sample_negOuter) * 0.25 + rgb_two_tab * 0.5;

	float luma_four_tab = dot(rgb_four_tab, to_luma);
	if (luma_four_tab < luma_min || luma_four_tab > luma_max)
		out_frag_color = vec4(rgb_two_tab, 1.0);
	else
		out_frag_color = vec4(rgb_four_tab, 1.0);

	#ifdef SHOW_EDGES
		out_frag_color.r = 1.0;
	#endif
}