#version 330 core
in vec4 ourColor;
in vec2 TexCoords;

out vec4 color;

uniform sampler2D Texture0;

void main()
{   
	color = ourColor;
}