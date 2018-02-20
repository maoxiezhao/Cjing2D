#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 aNormal;

uniform mat4 projection;

out vec3 FragPos;
out vec4 OurColor;
out vec2 TexCoords;
out vec3 Normal;

void main()
{
	gl_Position = vec4(position, 1.0) * projection;

	FragPos = position;
    TexCoords = texCoord;
	OurColor  = color;
	Normal = aNormal;
}