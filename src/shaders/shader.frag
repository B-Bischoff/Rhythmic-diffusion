#version 460 core

out vec4 FragColor;
in vec2 UVs;

uniform sampler2D screen;

void main()
{
	FragColor = texture(screen, UVs);
}
