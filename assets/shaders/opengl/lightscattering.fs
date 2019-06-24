#version 410

in vec2 vTexCoord;
layout(location = 0) out vec4 fragColor;

uniform float exposure;
uniform float decay;
uniform float density;
uniform float weight;
uniform vec2 lightPositionOnScreen;
uniform sampler2D uColorTexture;
uniform int samples;

void main()
{	
	vec2 deltaTextCoord = vec2(vTexCoord.st - lightPositionOnScreen.xy);
	vec2 textCoo = vTexCoord.st;
	deltaTextCoord *= 1.0 /  float(samples) * density;

	float illuminationDecay = 1.0;

	for(int i=0; i < samples ; i++) {
        textCoo -= deltaTextCoord;
        vec4 rgbaSample = texture2D(uColorTexture, textCoo);
    
        rgbaSample *= illuminationDecay * weight;
        fragColor += rgbaSample;
        illuminationDecay *= decay;
    }
    fragColor *= exposure;
    //fragColor = texture2D(uColorTexture, vTexCoord.st);
}
