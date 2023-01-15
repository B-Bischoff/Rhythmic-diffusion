#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D texture;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 existingPixel = imageLoad(texture, pixel_coords);

	int RADIUS = 50;
	ivec2 center = ivec2(1920 / 2, 1080 / 2);
	int x = center.x - pixel_coords.x;
	x = x * x;
	int y = center.y - pixel_coords.y;
	y = y * y;
	if (x + y < RADIUS * RADIUS)
	{
		existingPixel.b = 1;
	}

	imageStore(texture, pixel_coords, existingPixel);
}