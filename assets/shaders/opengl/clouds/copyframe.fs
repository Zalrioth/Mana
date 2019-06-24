#version 410 core

//https://raw.githubusercontent.com/fede-vaccaro/TerrainEngine-OpenGL/master/shaders/copyFrame.frag

//layout (location = 0) out vec4 color;
//layout (location = 1) out vec4 alphaness;
//layout (location = 2) out vec4 disc1;
//layout (location = 3) out vec4 disc2;
//layout (location = 4) out vec4 disc3;
layout (location = 0) out vec4 fragColor;
layout (location = 3) out vec4 lightScatteringColor;

in vec2 TexCoords;

uniform sampler2D colorTex;
uniform sampler2D alphanessTex;

void main()
{
	fragColor = texture(colorTex, TexCoords);
	lightScatteringColor = vec4(0.529, 0.808, 0.922, 1.0);
	lightScatteringColor.rgb *= 1.0 - texture(alphanessTex, TexCoords).r;
	//color = texture(colorTex, TexCoords);
	////alphaness = texture(alphanessTex, TexCoords);
	//alphaness = vec4(0,0,0,0);
	//disc1 = vec4(1,0,0,1);
	//disc2 = vec4(1,0,0,1);
	//disc3 = vec4(1,0,0,1);

	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}  