#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D currentScreen;
layout(rgba32f, binding = 1) uniform image2D nextScreen;
layout(rgba32f, binding = 2) uniform image2D paramScreen;

layout (location = 0) uniform float t;
layout (location = 1) uniform float _ReactionSpeed;
layout (location = 2) uniform vec4 _Properties;
// vec 4 properties :
// x = diffusion rate A
// y = diffusion rate B
// z = feed rate
// w = kill rate

vec3 laplacian(in ivec2 uv, in vec2 texelSize) {
	vec3 rg = vec3(0, 0, 0);

	rg += imageLoad(currentScreen, uv + ivec2(-1, -1)).rgb * 0.05;
	rg += imageLoad(currentScreen, uv + ivec2(-0, -1)).rgb * 0.2;
	rg += imageLoad(currentScreen, uv + ivec2(1, -1)).rgb * 0.05;
	rg += imageLoad(currentScreen, uv + ivec2(-1, 0)).rgb * 0.2;
	rg += imageLoad(currentScreen, uv + ivec2(0, 0)).rgb * -1;
	rg += imageLoad(currentScreen, uv + ivec2(1, 0)).rgb * 0.2;
	rg += imageLoad(currentScreen, uv + ivec2(-1, 1)).rgb * 0.05;
	rg += imageLoad(currentScreen, uv + ivec2(0, 1)).rgb * 0.2;
	rg += imageLoad(currentScreen, uv + ivec2(1, 1)).rgb * 0.05;

	return rg;
}

void main()
{
	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 existingPixel = imageLoad(currentScreen, pixel_coords);

	float diffusionRateA = _Properties.x;
	float diffusionRateB = _Properties.y;
	float feedRate = _Properties.z;
	//float killRate = _Properties.w;
	float killRate = imageLoad(paramScreen, pixel_coords).w / 100 * 15;

	float a = existingPixel.r;
	float b = existingPixel.b;

	vec2 texelSize = vec2(1.0, 1.0);
	vec3 lp = laplacian(pixel_coords, texelSize);
	float a2 = a + (diffusionRateA * lp.r - a*b*b + feedRate*(1 - a)) * _ReactionSpeed;
	float b2 = b + (diffusionRateB * lp.b + a*b*b - (killRate + feedRate)*b) * _ReactionSpeed;

	pixel = vec4(a2, 0, b2, 1.0);

	imageStore(nextScreen, pixel_coords, pixel);
}
