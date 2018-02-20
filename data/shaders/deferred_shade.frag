#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texturePosition;
uniform sampler2D textureNormal;
uniform sampler2D textureColor;
uniform sampler2D textureLight;

// light info
const int LIGHT_MAX_NUM = 32;
struct Light{
	vec3 Position;
	float Radius;
	vec3  Color;
	float Coeffi;
	float SquaredCoeffi;
};

uniform Light lights[LIGHT_MAX_NUM];
uniform int lightCount;
uniform vec3 ambientColor;	// 环境光颜色，目前仅仅用单一color代替
uniform int firstDraw;		// 仅在第一次渲染考虑环境光	

void main()
{   
	vec3 FragPos = texture(texturePosition, TexCoord).rgb;
	vec3 Normal = texture(textureNormal, TexCoord).rgb;
	vec3 Color = texture(textureColor, TexCoord).rgb;
	vec4 LightMaskColor  = texture(textureLight, TexCoord).rgba;

	int lightMask = int(LightMaskColor.r * 255) << 24 | 
					int(LightMaskColor.g * 255) << 16 |
					int(LightMaskColor.b * 255) << 8 |
					int(LightMaskColor.a * 255);

	// test for normal
	Normal = vec3(0.0,0.0,1.0);

    // light process
    vec3 curColor = firstDraw > 0 ? Color*ambientColor : vec3(0.0,0.0,0.0);
	for(int i = 0; i < lightCount; i++)
	{
		// 目前全部遍历，接下来最多传入32个光源
		// 根据光照贴图位操作判断
		bool inLight = ((lightMask >> i) & 1) > 0; 
		if(inLight){
			vec3 lightPos = lights[i].Position;
			float distance = length(lightPos - FragPos);
			if (distance <= lights[i].Radius)
			{	
				vec3 dir = normalize(lightPos - FragPos);
			    vec3 diffuse = max(dot(Normal, dir), 0.0) * lights[i].Color;
				float decay = 1.0 / (1 + lights[i].Coeffi * distance + lights[i].SquaredCoeffi * distance * distance);
				curColor += diffuse * decay;
			}
		}
	}

    // post process
    // 考虑泛光处理

	FragColor = vec4(curColor, 1.0);
}