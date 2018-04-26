#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D postBuffer;

// hdr 曝光参数
const float exposure = 1.0;
const float gamma = 2.2;

void main()
{   
	vec3 color = texture(postBuffer, TexCoord).rgb;
	// hdr
	vec3 mappingColor = vec3(1.0) - exp(-color * exposure);
	// gammma
	//mappingColor = pow(mappingColor, vec3(1.0 / gamma));
	FragColor = vec4(color, 1.0);
}