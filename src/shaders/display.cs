#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D inTexture;
layout(rgba32f, binding = 1) uniform image2D outTexture;
layout(rgba32f, binding = 2) uniform image2D paramTexture;

layout (location = 0) uniform vec3 colorA;
layout (location = 1) uniform vec3 colorB;

float invLerp(vec4 from, vec4 to, vec4 value)
{
	float diffX = (value.x - from.x) / (to.x - from.x);
	float diffY = (value.y - from.y) / (to.y - from.y);
	float diffZ = (value.z - from.z) / (to.z - from.z);
	return (diffX + diffY + diffZ) / 3;
}

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 existingPixel = imageLoad(inTexture, pixel_coords);

	existingPixel = mix(vec4(colorA, 1), vec4(colorB, 1), existingPixel.b);
	existingPixel = mix(existingPixel, vec4(1.0, 0.0, 0.0, 0.0), imageLoad(paramTexture, pixel_coords).r);
	existingPixel = mix(existingPixel, vec4(0.0, 1.0, 0.0, 0.0), imageLoad(paramTexture, pixel_coords).g);
	existingPixel = mix(existingPixel, vec4(0.0, 0.0, 1.0, 0.0), imageLoad(paramTexture, pixel_coords).b);

	imageStore(outTexture, pixel_coords, existingPixel);
}
