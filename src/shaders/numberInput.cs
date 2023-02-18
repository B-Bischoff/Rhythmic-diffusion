#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D texture;

layout (location = 0) uniform int channel;
layout (location = 1) uniform float value;

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 pixel = imageLoad(texture, pixel_coords);

	pixel[channel] = value;

	imageStore(texture, pixel_coords, pixel);
}
