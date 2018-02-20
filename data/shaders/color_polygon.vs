#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform mat4 projection;

out vec4 vColor;

void main()
{
	gl_Position = vec4(position, 1.0) * projection;
	vColor  = color;
}
