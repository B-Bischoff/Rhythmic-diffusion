#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D inTexture;
layout(rgba32f, binding = 1) uniform image2D outTexture;
layout(rgba32f, binding = 2) uniform image2D paramTexture;

#define MAX_COLOR 10

/* 
 * vec4 is used to keep track of a color and its gradient position as follows
 *	x : Red | y : Blue | z : Green | w : Gradient position (between 0 and 1)
*/
uniform vec4 gradient[10];
uniform int colorNumber;
uniform vec4 visualizeChannels;

float invLerp(vec4 from, vec4 to, vec4 value)
{
	float diffX = (value.x - from.x) / (to.x - from.x);
	float diffY = (value.y - from.y) / (to.y - from.y);
	float diffZ = (value.z - from.z) / (to.z - from.z);
	return (diffX + diffY + diffZ) / 3;
}

vec4 visualizeParamTexture(vec4 paramPixel);

int findColors(float t)
{
	int i = 0;
	while (t >= gradient[i].w && i < colorNumber - 1)
		i++;
	return i;
}

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 existingPixel = imageLoad(inTexture, pixel_coords);

	int index = findColors(existingPixel.b);
	float t = (existingPixel.b - gradient[index - 1].w) / (gradient[index].w - gradient[index - 1].w);
	vec3 rgb = mix(vec3(gradient[index - 1].xyz), vec3(gradient[index].xyz), t);
	existingPixel = vec4(rgb, 1.0);

	if (visualizeChannels != vec4(0))
	{
		vec4 paramPixel = visualizeParamTexture(imageLoad(paramTexture, pixel_coords));
		existingPixel = mix(paramPixel, existingPixel, 0.5);
	}

	imageStore(outTexture, pixel_coords, existingPixel);
}

vec4 visualizeParamTexture(vec4 paramPixel)
{
	vec4 pixel = vec4(0, 0, 0, 1);

	if (visualizeChannels.x == 1)
		pixel.r = paramPixel.x;
	if (visualizeChannels.y == 1)
		pixel.g = paramPixel.y;
	if (visualizeChannels.z == 1)
		pixel.b = paramPixel.z;
	if (visualizeChannels.w == 1)
	{
		if (pixel.r < paramPixel.w)
			pixel.r = paramPixel.w;
		if (pixel.g < paramPixel.w)
			pixel.g = paramPixel.w;
	}
	return pixel;
}
