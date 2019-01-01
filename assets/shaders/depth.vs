//http://glampert.com/2014/01-26/visualizing-the-depth-buffer/

#version 410

out vec2 vTexCoord;

void main(void)
{
	vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.0, 1.0), vec4(1.0, -1.0, 0.0, 1.0), vec4(-1.0, 1.0, 0.0, 1.0), vec4(1.0, 1.0, 0.0, 1.0));
	vec2 texCoord[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

	vTexCoord = texCoord[gl_VertexID];
	gl_Position = vertices[gl_VertexID];
}
