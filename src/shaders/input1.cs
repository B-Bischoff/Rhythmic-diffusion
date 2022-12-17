#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D texture;

float PI = 3.1415;

layout (location = 0) uniform float time;

float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
	float unit = 1920/freq;
	vec2 ij = floor(p/unit);
	vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(PI*xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	for (int i = 0; i<50; i++){
		n+=amp*noise(p, f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
}
void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 existingPixel = imageLoad(texture, pixel_coords);

	// Square
	//if (pixel_coords.x > 350 && pixel_coords.x < 450 && pixel_coords.y > 350 && pixel_coords.y < 450)
	//	existingPixel.b = 1;
	//if (pixel_coords.x > 950 && pixel_coords.x < 1050 && pixel_coords.y > 950 && pixel_coords.y < 1050)
	//	existingPixel.b = 1;

	// Circle
	//int RADIUS =50;
	//ivec2 center = ivec2(1920 / 2, 1080 / 2);
	//int x = center.x - pixel_coords.x;
	//x = x * x;
	//int y = center.y - pixel_coords.y;
	//y = y * y;
	//if (x + y < RADIUS * RADIUS)
	//{
	//	existingPixel.b = 1;
	//}

	const float SCALE = 10;
	float h = pNoise(vec2(pixel_coords.x, pixel_coords.y) * SCALE, 4);

	//existingPixel.r += clamp(h * 0.5 + existingPixel.g, 0, 1);
	//existingPixel.g += clamp(h * 0.5 + existingPixel.g, 0, 1);
	existingPixel.b = clamp(h * 0.001 + existingPixel.b, 0, 1);


	imageStore(texture, pixel_coords, existingPixel);
}
