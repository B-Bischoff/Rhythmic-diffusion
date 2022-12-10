#version 330

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

flat out vec3 fragmentColor;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * transform * vec4(vertexPosition, 1.0f);
	fragmentColor = vertexColor;
}
