#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D inTexture;
layout(rgba32f, binding = 1) uniform image2D outTexture;
layout(rgba32f, binding = 2) uniform image2D paramTexture;

layout (location = 0) uniform vec3 colorA;
layout (location = 1) uniform vec3 colorB;
layout (location = 2) uniform vec4 visualizeChannels;

float invLerp(vec4 from, vec4 to, vec4 value)
{
	float diffX = (value.x - from.x) / (to.x - from.x);
	float diffY = (value.y - from.y) / (to.y - from.y);
	float diffZ = (value.z - from.z) / (to.z - from.z);
	return (diffX + diffY + diffZ) / 3;
}

vec4 visualizeParamTexture(vec4 paramPixel);

int findColors(float thresholds[10], int thresholdNumber, float t)
{
	int i = 0;
	while (t >= thresholds[i] && i < thresholdNumber - 1)
		i++;
	return i;
}

void main()
{
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec4 existingPixel = imageLoad(inTexture, pixel_coords);

	//existingPixel = mix(vec4(colorA, 1), vec4(colorB, 1), existingPixel.b);

	// Some interesting resuts:
	//vec4 color0 = vec4(0, 0, 0, 1);
	//vec4 color1 = vec4(1, 0, 0, 1);
	//vec4 color2 = vec4(0, 0, 1, 1);
	//if (existingPixel.b < 0.1)
	//	existingPixel = mix(color0, color1, existingPixel.b);
	//else
	//	existingPixel = mix(color1, color2, existingPixel.b);

	// Testing
	clamp(existingPixel.b, 0, 1);
	const int N = 7;
	float thresholds[10];
	for (int i = 0; i < 10; i++)
		thresholds[i] = 0;
	thresholds[0] = 0.0;
	thresholds[1] = 0.11;
	thresholds[2] = 0.21;
	thresholds[3] = 0.31;
	thresholds[4] = 0.41;
	thresholds[5] = 0.51;
	thresholds[6] = 1.0;
	vec4 colors[10];
	for (int i = 0; i < 10; i++)
		colors[i] = vec4(0);
	colors[0] = vec4(0.1,0.1,0.1,1);
	colors[1] = vec4(0,1,0,1);
	colors[2] = vec4(0,0,1,1);
	colors[3] = vec4(1,0,1,1);
	colors[4] = vec4(0,1,1,1);
	colors[5] = vec4(1,1,0,1);
	colors[6] = vec4(1,1,1,1);
	int index = findColors(thresholds, N, existingPixel.b);
	//float t = mix(thresholds[index - 1], thresholds[index], existingPixel.b);
	float t = (existingPixel.b - thresholds[index - 1]) / (thresholds[index] - thresholds[index - 1]);
	existingPixel = mix(colors[index - 1], colors[index], t);

	// End testing

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
