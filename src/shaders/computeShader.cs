#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D currentScreen;
//layout(rgba32f, binding = 1) uniform image2D nextScreen;

layout (location = 0) uniform float t;
layout (location = 1) uniform float _ReactionSpeed;
layout (location = 2) uniform float _DiffusionRateA;
layout (location = 3) uniform float _DiffusionRateB;
layout (location = 4) uniform float _FeedRate;
layout (location = 5) uniform float _KillRate;

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

float invLerp(vec4 from, vec4 to, vec4 value)
{
	float diffX = (value.x - from.x) / (to.x - from.x);
	float diffY = (value.y - from.y) / (to.y - from.y);
	float diffZ = (value.z - from.z) / (to.z - from.z);
	return (diffX + diffY + diffZ) / 3;
}

void main()
{
	vec4 colorB = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 colorA = vec4(0.0, 1.0, 0.0, 1.0);

	vec4 pixel = vec4(0.0, 1.0, 0.0, 1.0);

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 existingPixel = imageLoad(currentScreen, pixel_coords);

	if (pixel_coords.x > 350 && pixel_coords.x < 450 && pixel_coords.y > 350 && pixel_coords.y < 450)
		existingPixel = colorB;
	if (pixel_coords.x > 950 && pixel_coords.x < 1050 && pixel_coords.y > 950 && pixel_coords.y < 1050)
		existingPixel = colorB;

	float a = existingPixel.r;
	float b = existingPixel.g;
	//float a = invLerp(colorA, colorB, existingPixel);
	//float b = 1 - a;

	vec2 texelSize = vec2(1.0, 1.0);
	vec3 lp = laplacian(pixel_coords, texelSize);
	float a2 = a + (_DiffusionRateA * lp.x - a*b*b + _FeedRate*(1 - a)) * _ReactionSpeed;
	float b2 = b + (_DiffusionRateB * lp.y + a*b*b - (_KillRate + _FeedRate)*b) * _ReactionSpeed;

	pixel.x = a2;
	pixel.y = b2;
	//pixel.z = (a2 + b2) / 2;

	//vec4 t = vec4(a2, a2, a2, 1);
	//pixel = mix(colorA, colorB, t);

	imageStore(currentScreen, pixel_coords, pixel);
}
