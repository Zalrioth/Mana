#version 330

// Bokeh dof
//https://vimeo.com/68658109
// dof
//http://artmartinsh.blogspot.com/2010/02/glsl-lens-blur-filter-with-bokeh.html
// dof with bokeh
//https://github.com/orthecreedence/ghostie/blob/master/opengl/glsl/dof.bokeh.2.1.frag
// newer version
//https://github.com/orthecreedence/ghostie/blob/master/opengl/glsl/dof.bokeh.2.4.frag
//http://devlog-martinsh.blogspot.com/2011/12/glsl-depth-of-field-with-bokeh-v24.html
//http://devlog-martinsh.blogspot.com/search/label/bokeh
//https://github.com/NatronGitHub/openfx-misc/blob/master/Shadertoy/presets/default/DoF_bokeh_2.4-natron.frag.glsl
// dof settings
//https://facepunch.com/showthread.php?t=1401594&p=45109791&viewfull=1
// looks good
//https://mynameismjp.wordpress.com/2011/02/28/bokeh/
// witcher 2 style bokeh
//https://bartwronski.com/2014/04/07/bokeh-depth-of-field-going-insane-part-1/
// looks good but in directx
//https://mynameismjp.wordpress.com/2011/02/28/bokeh/
// good tester
//http://www.andrewberg.com/prototypes/threejs/bokeh/
//https://github.com/mrdoob/three.js/issues/3182
// gpu gems dof
//https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch28.html
// how amazon does dof and motion blur
//https://docs.aws.amazon.com/lumberyard/latest/userguide/rendering-graphics-cameras.html


/*
DoF with bokeh GLSL shader v2.4
by Martins Upitis (martinsh) (devlog-martinsh.blogspot.com)
----------------------
The shader is Blender Game Engine ready, but it should be quite simple to adapt for your engine.
This work is licensed under a Creative Commons Attribution 3.0 Unported License.
So you are free to share, modify and adapt it for your needs, and even use it for commercial use.
I would also love to hear about a project you are using it.
Have fun,
Martins
----------------------
changelog:

2.4:
- physically accurate DoF simulation calculated from "focalDepth" ,"focalLength", "f-stop" and "CoC" parameters.
- option for artist controlled DoF simulation calculated only from "focalDepth" and individual controls for near and far blur
- added "circe of confusion" (CoC) parameter in mm to accurately simulate DoF with different camera sensor or film sizes
- cleaned up the code
- some optimization
2.3:
- new and physically little more accurate DoF
- two extra input variables - focal length and aperture iris diameter
- added a debug visualization of focus point and focal range
2.1:
- added an option for pentagonal bokeh shape
- minor fixes
2.0:
- variable sample count to increase quality/performance
- option to blur depth buffer to reduce hard edges
- option to dither the samples with noise or pattern
- bokeh chromatic aberration/fringing
- bokeh bias to bring out bokeh edges
- image thresholding to bring out highlights when image is out of focus
*/

uniform sampler2D renderTex;
uniform sampler2D depthTex;

smooth in vec2 texcoord;
out vec4 fragColor;
//uniform float renderTexWidth;
//uniform float renderTexHeight;

#define PI  3.14159265

//float width = renderTexWidth; //texture width
//float height = renderTexHeight; //texture height
float width = 800; //texture width
float height = 600; //texture height

vec2 texel = vec2(1.0/width,1.0/height);

//uniform variables from external script

/*
uniform float focalDepth;  //focal distance value in meters, but you may use autofocus option below
uniform float focalLength; //focal length in mm
uniform float fstop; //f-stop value
uniform bool showFocus; //show debug focus point and focal range (red = focal point, green = focal range)
*/
/*float focalDepth = 1.5;
float focalLength = 2.0;
float fstop = 1.0;
bool showFocus = false;*/

uniform float focalDepth ;  //focal distance value in meters, but you may use autofocus option below
uniform float focalLength; //focal length in mm
uniform float fstop; //f-stop value
bool showFocus = false; //show debug focus point and focal range (red = focal point, green = focal range)

/*
make sure that these two values are the same for your camera, otherwise distances will be wrong.
*/

float znear = 0.01; //camera clipping start
float zfar = 1000.0; //camera clipping end

//------------------------------------------
//user variables

//GUIDE HOW TO USE
//http://devlog-martinsh.blogspot.com/2011/11/glsl-depth-of-field-with-bokeh-v21.html

int samples = 5; //samples on the first ring
int rings = 14; //ring count

bool manualdof = false; //manual dof calculation
float ndofstart = 1.0; //near dof blur start
float ndofdist = 2.0; //near dof blur falloff distance
float fdofstart = 1.0; //far dof blur start
float fdofdist = 3.0; //far dof blur falloff distance

float CoC = 0.03;//circle of confusion size in mm (35mm film = 0.03mm)

bool vignetting = true; //use optical lens vignetting?
float vignout = 1.3; //vignetting outer border
float vignin = 0.0; //vignetting inner border
float vignfade = 22.0; //f-stops till vignete fades

bool autofocus = true; //use autofocus in shader? disable if you use external focalDepth value
vec2 focus = vec2(0.5,0.5); // autofocus point on screen (0.0,0.0 - left lower corner, 1.0,1.0 - upper right)
float maxblur = 1.0; //clamp value of max blur (0.0 = no blur,1.0 default)





float threshold = 0.5; //highlight threshold;
float gain = 1.0; //highlight gain;

float bias = 0.25; //bokeh edge bias
float fringe = 0.7; //bokeh chromatic aberration/fringing

bool noise = true; //use noise instead of pattern for sample dithering
float namount = 0.0001; //dither amount

bool depthblur = false; //blur the depth buffer?
float dbsize = 1.25; //depthblursize

/*
next part is experimental
not looking good with small sample and ring count
looks okay starting from samples = 4, rings = 4
*/

bool pentagon = true; //use pentagon as bokeh shape?
float feather = 3.0; //pentagon shape feather

//------------------------------------------


float penta(vec2 coords) //pentagonal shape
{
	float scale = float(rings) - 1.3;
	vec4  HS0 = vec4( 1.0,         0.0,         0.0,  1.0);
	vec4  HS1 = vec4( 0.309016994, 0.951056516, 0.0,  1.0);
	vec4  HS2 = vec4(-0.809016994, 0.587785252, 0.0,  1.0);
	vec4  HS3 = vec4(-0.809016994,-0.587785252, 0.0,  1.0);
	vec4  HS4 = vec4( 0.309016994,-0.951056516, 0.0,  1.0);
	vec4  HS5 = vec4( 0.0        ,0.0         , 1.0,  1.0);

	vec4  one = vec4( 1.0 );

	vec4 P = vec4((coords),vec2(scale, scale));

	vec4 dist = vec4(0.0);
	float inorout = -4.0;

	dist.x = dot( P, HS0 );
	dist.y = dot( P, HS1 );
	dist.z = dot( P, HS2 );
	dist.w = dot( P, HS3 );

	dist = smoothstep( -feather, feather, dist );

	inorout += dot( dist, one );

	dist.x = dot( P, HS4 );
	dist.y = HS5.w - abs( P.z );

	dist = smoothstep( -feather, feather, dist );
	inorout += dist.x;

	return clamp( inorout, 0.0, 1.0 );
}

float bdepth(vec2 coords) //blurring depth
{
	float d = 0.0;
	float kernel[9];
	vec2 offset[9];

	vec2 wh = vec2(texel.x, texel.y) * dbsize;

	offset[0] = vec2(-wh.x,-wh.y);
	offset[1] = vec2( 0.0, -wh.y);
	offset[2] = vec2( wh.x -wh.y);

	offset[3] = vec2(-wh.x,  0.0);
	offset[4] = vec2( 0.0,   0.0);
	offset[5] = vec2( wh.x,  0.0);

	offset[6] = vec2(-wh.x, wh.y);
	offset[7] = vec2( 0.0,  wh.y);
	offset[8] = vec2( wh.x, wh.y);

	kernel[0] = 1.0/16.0;   kernel[1] = 2.0/16.0;   kernel[2] = 1.0/16.0;
	kernel[3] = 2.0/16.0;   kernel[4] = 4.0/16.0;   kernel[5] = 2.0/16.0;
	kernel[6] = 1.0/16.0;   kernel[7] = 2.0/16.0;   kernel[8] = 1.0/16.0;


	for( int i=0; i<9; i++ )
	{
		float tmp = texture(depthTex, coords + offset[i]).r;
		d += tmp * kernel[i];
	}

	return d;
}


vec3 color(vec2 coords,float blur) //processing the sample
{
	vec3 col = vec3(0.0);

	col.r = texture(renderTex,coords + vec2(0.0,1.0)*texel*fringe*blur).r;
	col.g = texture(renderTex,coords + vec2(-0.866,-0.5)*texel*fringe*blur).g;
	col.b = texture(renderTex,coords + vec2(0.866,-0.5)*texel*fringe*blur).b;

	vec3 lumcoeff = vec3(0.299,0.587,0.114);
	float lum = dot(col.rgb, lumcoeff);
	float thresh = max((lum-threshold)*gain, 0.0);
	return col+mix(vec3(0.0),col,thresh*blur);
}

vec2 rand(vec2 coord) //generating noise/pattern texture for dithering
{
	float noiseX = ((fract(1.0-coord.s*(width/2.0))*0.25)+(fract(coord.t*(height/2.0))*0.75))*2.0-1.0;
	float noiseY = ((fract(1.0-coord.s*(width/2.0))*0.75)+(fract(coord.t*(height/2.0))*0.25))*2.0-1.0;

	if (noise)
	{
		noiseX = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
		noiseY = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;
	}
	return vec2(noiseX,noiseY);
}

vec3 debugFocus(vec3 col, float blur, float depth)
{
	float edge = 0.002*depth; //distance based edge smoothing
	float m = clamp(smoothstep(0.0,edge,blur),0.0,1.0);
	float e = clamp(smoothstep(1.0-edge,1.0,blur),0.0,1.0);

	col = mix(col,vec3(1.0,0.5,0.0),(1.0-m)*0.6);
	col = mix(col,vec3(0.0,0.5,1.0),((1.0-e)-(1.0-m))*0.2);

	return col;
}

float linearize(float depth)
{
	return -zfar * znear / (depth * (zfar - znear) - zfar);
}

float vignette()
{
	//float dist = distance(gl_TexCoord[3].xy, vec2(0.5,0.5));
	float dist = distance(texcoord.xy, vec2(0.5,0.5));
	dist = smoothstep(vignout+(fstop/vignfade), vignin+(fstop/vignfade), dist);
	return clamp(dist,0.0,1.0);
}

void main()
{
	//scene depth calculation

	float depth = linearize(texture(depthTex,texcoord.xy).x);

	if (depthblur)
	{
		depth = linearize(bdepth(texcoord.xy));
	}

	//focal plane calculation

	float fDepth = focalDepth;

	//if (autofocus)
	{
		fDepth = linearize(texture(depthTex,focus).x);
	}

	//dof blur factor calculation

	float blur = 0.0;

	if (manualdof)
	{
		float a = depth-fDepth; //focal plane
		float b = (a-fdofstart)/fdofdist; //far DoF
		float c = (-a-ndofstart)/ndofdist; //near Dof
		blur = (a>0.0)?b:c;
	}

	else
	{
		float f = focalLength; //focal length in mm
		float d = fDepth*1000.0; //focal plane in mm
		float o = depth*1000.0; //depth in mm

		float a = (o*f)/(o-f);
		float b = (d*f)/(d-f);
		float c = (d-f)/(d*fstop*CoC);

		blur = abs(a-b)*c;
	}

	blur = clamp(blur,0.0,1.0);

	////////////////////////////////////////////////

    //float range = 2500.0; //focal range
	//blur = clamp((abs(depth - fDepth)/range)*100.0,-maxblur,maxblur);

	////////////////////////////////////////////////

	// calculation of pattern for ditering

	vec2 noise = rand(texcoord.xy)*namount*blur;

	// getting blur x and y step factor

	float w = (1.0/width)*blur*maxblur+noise.x;
	float h = (1.0/height)*blur*maxblur+noise.y;

	// calculation of final color

	vec3 col = vec3(0.0);

	if(blur < 0.05) //some optimization thingy
	{
		col = texture(renderTex, texcoord.xy).rgb;
	}

	else
	{
		col = texture(renderTex, texcoord.xy).rgb;
		float s = 1.0;
		int ringsamples;

		for (int i = 1; i <= rings; i += 1)
		{
			ringsamples = i * samples;

			for (int j = 0 ; j < ringsamples ; j += 1)
			{
				float step = PI*2.0 / float(ringsamples);
				float pw = (cos(float(j)*step)*float(i));
				float ph = (sin(float(j)*step)*float(i));
				float p = 1.0;
				if (pentagon)
				{
					p = penta(vec2(pw,ph));
				}
				col += color(texcoord.xy + vec2(pw*w,ph*h),blur)*mix(1.0,(float(i))/(float(rings)),bias)*p;
				s += 1.0*mix(1.0,(float(i))/(float(rings)),bias)*p;
			}
		}
		col /= s; //divide by sample count
	}

	if (showFocus)
	{
		col = debugFocus(col, blur, depth);
	}

	if (vignetting)
	{
		col *= vignette();
	}

	//gl_FragColor.rgb = texture(renderTex, texcoord);
	fragColor.rgb = col;
	fragColor.a = 1.0;
}


















































/*in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D bgl_RenderedTexture;
uniform sampler2D bgl_DepthTexture;*/

//const float blurclamp = 3.0;  // max blur amount
//const float bias = 0.6; //aperture - bigger values for shallower depth of field
//uniform float focus;  // this value comes from ReadDepth script.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*uniform float renderTexWidth = 800;
uniform float renderTexHeight = 600;

#define PI  3.14159265

float width = renderTexWidth; //texture width
float height = renderTexHeight; //texture height

vec2 texel = vec2(1.0/width,1.0/height);

//uniform float focalDepth;  //external focal point value, but you may use autofocus option below
float focalDepth = 3.5;*/

//------------------------------------------
//user variables

/*int samples = 1; //samples on the first ring
int rings = 5; //ring count

bool autofocus = true; //use autofocus in shader? disable if you use external focalDepth value
vec2 focus = vec2(0.5,0.5); // autofocus point on screen (0.0,0.0 - left lower corner, 1.0,1.0 - upper right)
float range = 4.0; //focal range
float maxblur = 1.25; //clamp value of max blur

float threshold = 0.821; //highlight threshold;
float gain = 0.0; //highlight gain;

float bias = 10.4; //bokeh edge bias
float fringe = 1.0; //bokeh chromatic aberration/fringing

bool noise = false; //use noise instead of pattern for sample dithering
float namount = 0.0001; //dither amount

bool depthblur = false; //blur the depth buffer?
float dbsize = 1.0; //depthblursize

//------------------------------------------


float bdepth(vec2 coords) //blurring depth
{
	float d = 0.0;
	float kernel[9];
	vec2 offset[9];

	vec2 wh = vec2(texel.x, texel.y) * dbsize;

	offset[0] = vec2(-wh.x,-wh.y);
	offset[1] = vec2( 0.0, -wh.y);
	offset[2] = vec2( wh.x -wh.y);

	offset[3] = vec2(-wh.x,  0.0);
	offset[4] = vec2( 0.0,   0.0);
	offset[5] = vec2( wh.x,  0.0);

	offset[6] = vec2(-wh.x, wh.y);
	offset[7] = vec2( 0.0,  wh.y);
	offset[8] = vec2( wh.x, wh.y);

	kernel[0] = 1.0/16.0;   kernel[1] = 2.0/16.0;   kernel[2] = 1.0/16.0;
	kernel[3] = 2.0/16.0;   kernel[4] = 4.0/16.0;   kernel[5] = 2.0/16.0;
	kernel[6] = 1.0/16.0;   kernel[7] = 2.0/16.0;   kernel[8] = 1.0/16.0;


	for( int i=0; i<9; i++ )
	{
		float tmp = texture(bgl_RenderedTexture, coords + offset[i]).r;
		d += tmp * kernel[i];
	}

	return d;
}


vec3 color(vec2 coords,float blur) //processing the sample
{
	vec3 col = vec3(0.0);

	col.r = texture(bgl_RenderedTexture,coords + vec2(0.866,-0.5)*texel*fringe*blur).r;
	col.g = texture(bgl_RenderedTexture,coords + vec2(-0.866,-0.5)*texel*fringe*blur).g;
	col.b = texture(bgl_RenderedTexture,coords + vec2(0.866,-0.5)*texel*fringe*blur).b;

	vec3 lumcoeff = vec3(0.299,0.587,0.114);
	float lum = dot(col.rgb, lumcoeff);
	float thresh = max((lum-threshold)*gain, 0.0);
	return col+mix(vec3(0.0),col,thresh*blur);
}

vec2 rand(in vec2 coord) //generating noise/pattern texture for dithering
{
	float noiseX = ((fract(1.0-coord.s*(width/2.0))*0.25)+(fract(coord.t*(height/2.0))*0.75))*2.0-1.0;
	float noiseY = ((fract(1.0-coord.s*(width/2.0))*0.75)+(fract(coord.t*(height/2.0))*0.25))*2.0-1.0;

	if (noise)
	{
		noiseX = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
		noiseY = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;
	}
	return vec2(noiseX,noiseY);
}

void main()
{

	float depth = texture(bgl_RenderedTexture,vTexCoord.xy).x;
	float blur = 0.0;

	if (depthblur)
	{
		depth = bdepth(vTexCoord.xy);
	}

	blur = clamp((abs(depth - focalDepth)/range)*100.0,-maxblur,maxblur);

	if (autofocus)
	{
		float fDepth = texture(bgl_RenderedTexture,focus).x;
		blur = clamp((abs(depth - fDepth)/range)*100.0,-maxblur,maxblur);
	}

	vec2 noise = rand(vTexCoord.xy)*namount*blur;

	float w = (1.0/width)*blur+noise.x;
	float h = (1.0/height)*blur+noise.y;

	vec3 col = texture(bgl_RenderedTexture, vTexCoord.xy).rgb;
	float s = 1.0;

	int ringsamples;

	for (int i = 1; i <= rings; i += 1)
	{
		ringsamples = i * samples;

		for (int j = 0 ; j < ringsamples ; j += 1)
		{
			float step = PI*2.0 / float(ringsamples);
			float pw = (cos(float(j)*step)*float(i));
			float ph = (sin(float(j)*step)*float(i));
			float p = 1.0;
			col += color(vTexCoord.xy + vec2(pw*w,ph*h),blur)*mix(1.0,(float(i))/(float(rings)),bias)*p;
			s += 1.0*mix(1.0,(float(i))/(float(rings)),bias)*p;
		}
	}


	col /= s;

	fragColor.rgb = col;
	fragColor.a = 1.0;
}*/

/*void main()
{

        float aspectratio = 800.0 / 600.0;
        vec2 aspectcorrect = vec2(1.0, aspectratio);

        vec4 depth1   = texture(bgl_DepthTexture, vTexCoord);

        float factor = (depth1.x - focus);

        vec2 dofblur = vec2 (clamp(factor * bias, -blurclamp, blurclamp));

        vec4 col = vec4(0.0);

        col += texture(bgl_RenderedTexture, vTexCoord);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur);

        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.15,0.37 )*aspectcorrect) * dofblur*0.9);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.37,0.15 )*aspectcorrect) * dofblur*0.9);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.37,-0.15 )*aspectcorrect) * dofblur*0.9);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.15,-0.37 )*aspectcorrect) * dofblur*0.9);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.15,0.37 )*aspectcorrect) * dofblur*0.9);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.37,0.15 )*aspectcorrect) * dofblur*0.9);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.37,-0.15 )*aspectcorrect) * dofblur*0.9);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.15,-0.37 )*aspectcorrect) * dofblur*0.9);

        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.7);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.7);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.7);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.7);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.7);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.7);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.7);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.7);

        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.29,0.29 )*aspectcorrect) * dofblur*0.4);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.4,0.0 )*aspectcorrect) * dofblur*0.4);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.29,-0.29 )*aspectcorrect) * dofblur*0.4);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.0,-0.4 )*aspectcorrect) * dofblur*0.4);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.29,0.29 )*aspectcorrect) * dofblur*0.4);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.4,0.0 )*aspectcorrect) * dofblur*0.4);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( -0.29,-0.29 )*aspectcorrect) * dofblur*0.4);
        col += texture(bgl_RenderedTexture, vTexCoord + (vec2( 0.0,0.4 )*aspectcorrect) * dofblur*0.4);

        fragColor = col / 41.0;
        fragColor.a = 1.0;

        //gl_FragColor = col/41.0;
        //gl_FragColor.a = 1.0;
}*/