#version 330 core
in vec4 ourColor;
in vec2 TexCoords;
in float  chanel;
out vec4 color;

uniform sampler2D Texture0;

void main()
{   
	int type = int(chanel);
	float fontColor = texture(Texture0,TexCoords).r;
	if(type == 1)
	{
		fontColor = texture(Texture0,TexCoords).g;
	}
	else if(type == 2)
	{
		fontColor = texture(Texture0,TexCoords).b;
	}
	else if(type == 3)
	{
		fontColor = texture(Texture0,TexCoords).a;
	}
	
	vec4 sampled = vec4(1.0,1.0,1.0,fontColor);
	color = ourColor * sampled;
}