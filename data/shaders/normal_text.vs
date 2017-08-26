#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;

uniform mat4 projection;

out vec4 ourColor;
out vec2 TexCoords;
out float  chanel;

void main()
{
	gl_Position = vec4(position.xy, 0.0, 1.0) * projection;
    TexCoords = texCoord;
	ourColor  = color;
	chanel = position.z;
}