#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D screen;
layout (location = 0) uniform float t;

vec3 laplacian(in ivec2 uv, in vec2 texelSize) {
	vec3 rg = vec3(0, 0, 0);

	rg += imageLoad(screen, uv + ivec2(-1, -1)).rgb * 0.05;
	rg += imageLoad(screen, uv + ivec2(-0, -1)).rgb * 0.2;
	rg += imageLoad(screen, uv + ivec2(1, -1)).rgb * 0.05;
	rg += imageLoad(screen, uv + ivec2(-1, 0)).rgb * 0.2;
	rg += imageLoad(screen, uv + ivec2(0, 0)).rgb * -1;
	rg += imageLoad(screen, uv + ivec2(1, 0)).rgb * 0.2;
	rg += imageLoad(screen, uv + ivec2(-1, 1)).rgb * 0.05;
	rg += imageLoad(screen, uv + ivec2(0, 1)).rgb * 0.2;
	rg += imageLoad(screen, uv + ivec2(1, 1)).rgb * 0.05;

	return rg;
}

void main()
{
	//float speed = 100;
	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

	//pixel.x = mod(pixel_coords.x + t * speed, 800.0) / 800.0;
	//pixel.y = (pixel_coords.y) / 600.0;

	//pixel.x = mod(existingPixel.x + 0.0001, 1);
	//pixel.y = mod(existingPixel.y + 0.0001, 1);

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 existingPixel = imageLoad(screen, pixel_coords); // * 4;

	if (pixel_coords.x > 350 && pixel_coords.x < 450 && pixel_coords.y > 350 && pixel_coords.y < 450)
		existingPixel.g = 0.5;
	if (pixel_coords.x > 950 && pixel_coords.x < 1050 && pixel_coords.y > 950 && pixel_coords.y < 1050)
		existingPixel.g = 0.5;

	float a = existingPixel.r;
	float b = existingPixel.g;

	float _DiffusionRateA = 1;
	float _DiffusionRateB = 0.4;
	float _FeedRate = 0.028;
	float _KillRate = 0.06;
	float _ReactionSpeed = 1;

	vec2 texelSize = vec2(1.0, 1.0);
	vec3 lp = laplacian(pixel_coords, texelSize);
	float a2 = a + (_DiffusionRateA * lp.x - a*b*b + _FeedRate*(1 - a)) * _ReactionSpeed;
	float b2 = b + (_DiffusionRateB * lp.y + a*b*b - (_KillRate + _FeedRate)*b) * _ReactionSpeed;

	pixel.x = a2;
	pixel.y = b2;
	pixel.z = b2 /2;

	//pixel = pixel * 0.25;

	imageStore(screen, pixel_coords, pixel);
}


