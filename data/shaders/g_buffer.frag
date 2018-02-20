#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gColor;

in vec3 FragPos;
in vec4 OurColor;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D Texture0;

void main()
{   
	// 对于延迟着色，无法实现半透明，在该片段处理中
	// 首先将alpha为0的值剔除掉
	vec4 texColor = texture(Texture0, TexCoords);
	if(texColor.a < 0.1f)
		discard;
		
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gColor = texture(Texture0, TexCoords).rgb;
}